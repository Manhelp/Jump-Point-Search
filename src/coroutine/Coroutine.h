#pragma once

#include <iostream>
#include <thread>
#include <chrono>
#include <functional>
#include <coroutine>
#include <stdexcept>


struct coroutine_awaitable
{
	virtual bool await_ready() const { return false; }
	virtual void await_resume() {}
	/**
	 * 注意，因为协程在进入 awaiter.await_suspend() 前已完全暂停，所以该函数可以自由地在线程间转移协程柄，而无需额外同步。
	 * 例如，可以将它放入回调，将它调度成在异步 I/O 操作完成时在线程池上运行等。该情况下，因为当前协程可能已被恢复，从而执行了等待器的析构函数，
	 * 同时由于 await_suspend() 在当前线程上持续执行， await_suspend() 应该把 *this 当作已被销毁并且在柄被发布到其他线程后不再访问它。
	 */
	virtual void await_suspend(std::coroutine_handle<> handle)
	{
		handle.resume();
	}
};

struct coroutine_task
{
	struct promise_type {
		static coroutine_task get_return_object_on_allocation_failure() { return {}; }
		coroutine_task get_return_object() { return {}; }
		std::suspend_never initial_suspend() { return {}; }
		std::suspend_never final_suspend() noexcept { return {}; }
		void return_void() {}
		void unhandled_exception() {}
	};
};


