#include <iostream>
#include <iomanip>
#include <vector>

#include <memory>

#include <coroutine>

static std::vector<std::pair<int*, std::coroutine_handle<> > > g_test_rpc_manager;
static int g_test_rpc_fake_data = 0;

struct test_rpc_generator {
    struct test_rpc_data {
        int final_value;
        int yield_times;

        std::vector<std::coroutine_handle<> > follower;
    };

    struct promise_type;
    using data_ptr = std::shared_ptr<test_rpc_data>;

    struct promise_type {
        data_ptr data;
        static auto get_return_object_on_allocation_failure() {
            return test_rpc_generator{ nullptr };
        }

        auto get_return_object() {
            data = std::make_shared<test_rpc_data>();
            if (data) {
                data->final_value = 0;
                data->yield_times = 0;
            }
            return test_rpc_generator{ data };
        }

        auto initial_suspend() {
            return std::suspend_never{}; // STL提供了一些自带的awaiter实现，我们其实很多情况下也不需要另外写，直接用STL就好了
        }

        auto final_suspend() {
            return std::suspend_always{}; // 和上面一样，也是STL自带的awaiter实现
        }

        void unhandled_exception() {
            std::terminate();
        }

        // 用以支持 co_return
        void return_value(int v) {
            // 最终co_return时保存最终数据
            if (data) {
                data->final_value = v;

                auto followers = std::move(data->follower);
                for (auto& h : followers) {
                    h.resume();
                }
            }
        }

        // 用以支持 co_yield
        auto yield_value(int* value) {
            // 每次调用都会执行,创建handle用以后面恢复数据
            g_test_rpc_manager.emplace_back(std::make_pair(value, std::coroutine_handle<>::from_address(
                std::coroutine_handle<promise_type>::from_promise(*this).address()
            )));

            if (data) {
                ++data->yield_times;
            }

            return std::suspend_always{};
        }
    };

    // 下面的接入用侵入式的方式支持 co_await test_rpc_generator
    // MSVC 目前支持使用非侵入式的方式实现，但是clang不支持
    bool await_ready() noexcept {
        return value() > 0;
    }

    void await_resume() {
        std::cout << std::setw(32) << __FUNCTION__ << std::setw(3) << ": " << __LINE__ << ": test_rpc_generator resume for " << yield_times() << " time(s)" << std::endl;
    }

    void await_suspend(std::coroutine_handle<> h) {
        std::cout << std::setw(32) << __FUNCTION__ << std::setw(3) << ": " << __LINE__ << ": test_rpc_generator yield for " << yield_times() << " time(s), wait for " << h.address() << std::endl;

        // 记录要恢复父协程
        if (h) {
            add_follower(h);
        }
    }

    int value() const {
        if (data) {
            return data->final_value;
        }
        return 0;
    }

    int yield_times() const {
        if (data) {
            return data->yield_times;
        }

        return -1;
    }

    void add_follower(std::coroutine_handle<> h) {
        if (data) {
            data->follower.emplace_back(std::move(h));
        }
    }
private:
    test_rpc_generator(data_ptr d) : data(d) {}
    data_ptr data;
};

// 异步协程函数
test_rpc_generator f() {
    int rpc_res1, rpc_res2;
    co_yield &rpc_res1;
    // _alloca(rpc_res1);
    std::cout << "resumed got rpc_res1: " << rpc_res1 << "(@" << &rpc_res1 << ")" << std::endl;

    co_yield &rpc_res2;
    // _alloca(rpc_res2);
    std::cout << "resumed got rpc_res1: " << rpc_res1 << "(@" << &rpc_res1 << ")" << ", rpc_res2: " << rpc_res2 << "(@" << &rpc_res2 << ")" << std::endl;

    // 模拟多次RPC然后返回最终结果
    co_return rpc_res1 * 100 + rpc_res2;
}

// 这里模拟生成数据
void test_rpc_manager_run() {
    std::vector<std::pair<int*, std::coroutine_handle<> > > rpc_manager;
    g_test_rpc_manager.swap(rpc_manager);

    for (auto& generator : rpc_manager) {
        if (generator.first) {
            *generator.first = ++g_test_rpc_fake_data;
        }

        if (generator.second && !generator.second.done()) {
            generator.second.resume();
        }
    }
}

struct test_task {
    using ptr_t = std::shared_ptr<test_task>;

    test_task(int ms) : status(0), max_status(ms) {}

    bool is_ready() const noexcept {
        return status >= max_status;
    }

    int status;
    int max_status;
};

struct test_task_future {
    struct promise_type;
    using ptr_t = std::shared_ptr<test_task>;
    using handle = std::coroutine_handle<promise_type>;

    struct promise_type {
        static auto get_return_object_on_allocation_failure() {
            return test_task_future{ nullptr };
        }

        auto get_return_object() {
            return test_task_future{ handle::from_promise(*this) };
        }

        auto initial_suspend() {
            return std::suspend_never{};
        }

        auto final_suspend() {
            return std::suspend_always{};
        }

        void unhandled_exception() {
            std::terminate();
        }

        void return_void() {
            for (auto& h : follower) {
                h.resume();
            }
        }

        // 这里是为了把task::ptr_t数据关联进去，目前版本不支持promise构造函数参数，没想到什么其他好方法
        auto yield_value(ptr_t t) {
            task = t;
            return std::suspend_never{};
        }

        ptr_t task;
        std::vector<handle> follower;
    };

    // 下面的接入用侵入式的方式支持 co_await test_task::ptr_t
    struct awaitable {
        awaitable(const test_task_future& pt) {
            if (pt.coro) {
                data = pt.coro.promise().task;
                coro = pt.coro;
            }
        };

        bool await_ready() const {
            bool ret = !data || data->is_ready();
            if (ret) {
                std::cout << std::setw(32) << __FUNCTION__ << std::setw(3) << ": " << __LINE__ << ": task " << data.get() << " ready" << std::endl;
            }

            return ret;
        }

        void await_resume() {
            if (data) {
                ++data->status;
                std::cout << std::setw(32) << __FUNCTION__ << std::setw(3) << ": " << 
                    __LINE__ << ": task " << data.get() << " resume to " << (data ? data->status : -1) << std::endl;
            }
        }

        void await_suspend(handle h) {
            if (data) {
                std::cout << std::setw(32) << __FUNCTION__ << std::setw(3) << ": " << 
                    __LINE__ << ": task " << data.get() << " suspend test_task_future::handle from " << (data ? data->status : -1) <<
                    ", wait for " << h.address() << std::endl;

            }

            if (coro && h) {
                coro.promise().follower.emplace_back(h);
            }
        }

        ptr_t data;
        handle coro;
    };

    bool done() const {
        return !coro || coro.done();
    }

private:
    test_task_future(handle h) : coro(h) {}
    handle coro;
};

// 接入 co_await test_task::ptr_t
auto operator co_await(const test_task_future & pt) noexcept {
    return test_task_future::awaitable{ pt };
}

test_task_future h(test_task::ptr_t task) {
    // 这里是为了把task::ptr_t数据关联进去，目前版本不支持promise构造函数参数，没想到什么其他好方法
    co_yield task;

    // 模拟任务内部流程并调用外部RPC
    std::cout << std::setw(32) << __FUNCTION__ << std::setw(3) << ": " << __LINE__ << ": task " << task.get() << std::endl;
    test_rpc_generator rpc_res = f();
    co_await rpc_res;
    std::cout << std::setw(32) << __FUNCTION__ << std::setw(3) << ": " << __LINE__ << ": task " << task.get() << " call f() ret: " << rpc_res.value() << std::endl;
}

test_task_future g(test_task::ptr_t task) {
    // 这里是为了把task::ptr_t数据关联进去，目前版本不支持promise构造函数参数，没想到什么其他好方法
    co_yield task;

    // 等待子任务完成
    while (task && !task->is_ready()) {
        std::cout << std::setw(32) << __FUNCTION__ << std::setw(3) << ": " << __LINE__ << ": task " << task.get() << std::endl;
        co_await h(task);
    }
}

// int main(int argc, char* argv[]) {
// #ifdef __cpp_coroutines
//     std::cout << "__cpp_coroutines: " << __cpp_coroutines << std::endl;
// #endif

//     // 创建一个任务
//     test_task::ptr_t task = std::make_shared<test_task>(3);
//     // 运行任务
//     auto fut = g(task);
//     // 模拟从外部获取数据然会恢复协程
//     while (!fut.done()) {
//         test_rpc_manager_run();
//     }

//     return 0;
// }