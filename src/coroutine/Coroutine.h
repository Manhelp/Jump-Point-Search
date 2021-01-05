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


