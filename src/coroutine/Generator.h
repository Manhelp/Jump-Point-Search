#pragma once

#include <coroutine>
#include <iostream>
#include <optional>
#include <ranges>

/**
 * co_await
 * 一元运算符 co_await 暂停协程并将控制返回给调用方。
 * 其操作数是一个表达式，其类型必须:
 * 要么定义 operator co_await，
 * 要么能以当前协程的 Promise::await_transform 转换到这种类型。
 */

template <typename T> requires std::movable<T>
class Generator {
public:
    struct promise_type {
        /*
         * 调用 promise.get_return_object() 并将其结果在局部变量中保持。
         * 该调用的结果将在协程首次暂停时返回给调用方。
         * 至此并包含这个步骤为止，任何抛出的异常均传播回调用方，而非置于承诺中。
         */
        Generator<T> get_return_object() {
            return Generator{ Handle::from_promise(*this) };
        }

        /**
         * 堆分配
         * 若分配失败，则协程抛出 std::bad_alloc，除非 Promise 类型定义了成员函数
         * Promise::get_return_object_on_allocation_failure()。
         * 若定义了该成员函数，则使用 operator new 的 nothrow 形式进行分配，
         * 而在分配失败时，协程立即将从 Promise::get_return_object_on_allocation_failure() 获得的对象返回给调用方。
         */
        // static Generator<T> get_return_object_on_allocation_failure() {
        //     return Generator{ Handle::from_promise(*this) };
        // }

        /**
         * 调用 promise.initial_suspend() 并 co_await 其结果。典型的 Promise 类型，
         * (标准库定义了两个平凡的可等待体：std::suspend_always 及 std::suspend_never)
         * 要么（对于惰性启动的协程）返回 std::suspend_always，
         * 要么（对于急切启动的协程）返回 std::suspend_never 
         * 当 co_await promise.initial_suspend() 恢复时，开始协程体的执行。
         */
        static std::suspend_always initial_suspend() noexcept {
            return {};
        }
        /**
         * 以创建的逆序销毁所有具有自动存储期的变量。
         * 调用 promise.final_suspend() 并 co_await 其结果。
         */
        static std::suspend_always final_suspend() noexcept {
            return {};
        }
        /**
         * co_yield yield 表达式向调用方返回一个值并暂停当前协程：它是可恢复生成器函数的常用构建块
         * 它等价于 co_await promise.yield_value(表达式)
         * 典型的生成器的 yield_value 会将其实参存储（复制/移动或仅存储其地址，因为实参的生存期跨过 co_await 内的暂停点）
         * 到生成器对象中并返回 std::suspend_always，将控制转移给调用方/恢复方
         */
        std::suspend_always yield_value(T value) noexcept {
            current_value = std::move(value);
            return {};
        }

        // void return_value(T value) {
        //     current_value = std::move(value);
        // }

        // 生成器协程中不允许 co_await 。
        void await_transform() = delete;
        static void unhandled_exception() {
            throw;
        }

        std::optional<T> current_value;
    };

    using Handle = std::coroutine_handle<promise_type>;

    explicit Generator(Handle coroutine) : m_coroutine{coroutine} {}
    Generator() = default;
    ~Generator() {
        if(m_coroutine) {
            m_coroutine.destroy();
        }
    }
    Generator(const Generator&) = delete;
    Generator& operator=(const Generator&) = delete;
    Generator(Generator&& other) noexcept: m_coroutine{other.m_coroutine} {
        other.m_coroutine = {};
    }
    Generator& operator=(Generator&& other) noexcept {
        if(this != other) {
            m_coroutine = other.m_coroutine;
            other.m_coroutine = {};
        }
        return *this;
    }

     // 基于范围的 for 循环支持。
    class Iter {
    public:
        void operator ++() {
            m_coroutine.resume();
        }
        const T& operator*() const {
            return *m_coroutine.promise().current_value;
        }
        bool operator==(std::default_sentinel_t) const {
            return !m_coroutine || m_coroutine.done();
        }
        explicit Iter(Handle coroutine) : m_coroutine{coroutine} {}
    private:
        Handle m_coroutine;
    };

    Iter begin() {
        if(m_coroutine) {
            m_coroutine.resume();
        }
        return Iter{m_coroutine};
    }
    std::default_sentinel_t end() {
        return {};
    }
private:
    Handle m_coroutine;
};

template < std::integral T>
Generator<T> range(T first, T last) {
    while(first < last) {
        co_yield first++;
    }
}

