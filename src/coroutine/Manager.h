#pragma once

#include <coroutine>
#include <iostream>
#include <stdexcept>
#include <thread>
#include "Coroutine.h"
#include <unordered_map>
#include <mutex>
#include "Generator.h"

struct jthread_awaitable;

class thread_manager {
public:
    thread_manager();
    coroutine_task async_task();
public:
    static void main_thread_worker(thread_manager* manager);
    static void logic_thread_worker(thread_manager* manager);
    static jthread_awaitable await_suspend_handle(thread_manager& manager);
public:
    int m_global_index = 0;
    int64_t m_gen_task_tick = 0;

    std::jthread m_main_thread;
    std::jthread m_logic_thread;

    std::mutex m_main_mutex;
    std::unordered_map<uint, std::coroutine_handle<>> m_main_handles;
    std::unordered_map<uint, std::coroutine_handle<>> m_main_cache_handles;

    std::mutex m_logic_mutex;
    std::unordered_map<uint, std::coroutine_handle<>> m_logic_handles;
    std::unordered_map<uint, std::coroutine_handle<>> m_logic_cache_handles;
};

struct jthread_awaitable : public coroutine_awaitable {
    thread_manager* m_manager = nullptr;

    jthread_awaitable(thread_manager* manager) : m_manager{manager} {}
    void await_suspend(std::coroutine_handle<> handle) {
        std::lock_guard<std::mutex> guard(m_manager->m_logic_mutex);
        m_manager->m_logic_cache_handles[m_manager->m_global_index] = handle;

        std::cout << " input logic cache " << m_manager->m_global_index << " thread id " << std::this_thread::get_id() << std::endl;

        ++m_manager->m_global_index;
    }
};
