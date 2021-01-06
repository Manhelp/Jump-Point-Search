#pragma once

#include <coroutine>
#include <iostream>
#include <stdexcept>
#include <thread>
#include "Coroutine.h"


struct jthread_awaitable : public coroutine_awaitable {
    std::jthread* p_out;

    jthread_awaitable(std::jthread* out) : p_out{out} {}
    void await_suspend(std::coroutine_handle<> handle) {
        std::jthread& out = *p_out;
        if(out.joinable()) {
            throw std::runtime_error("Output jthread parameter not empty");
        }
        
        out = std::jthread([handle]{handle.resume();});
        // 潜在的未定义行为：访问潜在被销毁的 *this
        // std::cout << "New thread ID: " << p_out->get_id() << "\n";
        std::cout << "New thread ID: " << out.get_id() << std::endl;
    }
};

auto switch_to_new_thread(std::jthread& out) {
    return jthread_awaitable{&out};
}

coroutine_task resuming_on_new_thread(std::jthread& out) {
    std::cout << "Coroutine started on thread: " << std::this_thread::get_id() << std::endl;
    co_await switch_to_new_thread(out);
    // 等待器销毁于此
    std::cout << " Coroutine resumed on thread: " << std::this_thread::get_id() << std::endl;
}