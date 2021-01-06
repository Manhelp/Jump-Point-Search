#pragma once

#include <coroutine>
#include <iostream>
#include <stdexcept>
#include <thread>
#include "Coroutine.h"
#include <unordered_map>
#include <mutex>

struct jthread_awaitable;

class thread_manager {
public:
    thread_manager();
    coroutine_task init(int count = 0);
public:
    static void main_thread_worker(thread_manager* manager);
    static void logic_thread_worker(thread_manager* manager);
    static jthread_awaitable switch_to_new_thread(thread_manager& manager);
public:
    std::jthread m_main_thread;
    std::jthread m_logic_thread;
    
    std::mutex m_main_mutex;
    std::mutex m_logic_mutex;
    uint64_t m_global_index = 0;
    std::unordered_map<uint, std::coroutine_handle<>> m_main_handles;
    std::unordered_map<uint, std::coroutine_handle<>> m_main_cache_handles;
    std::unordered_map<uint, std::coroutine_handle<>> m_logic_handles;
    std::unordered_map<uint, std::coroutine_handle<>> m_logic_cache_handles;
};

struct jthread_awaitable : public coroutine_awaitable {
    thread_manager* m_manager = nullptr;

    jthread_awaitable(thread_manager* manager) : m_manager{manager} {}
    void await_suspend(std::coroutine_handle<> handle) {
        std::lock_guard<std::mutex> guard(m_manager->m_logic_mutex);
        m_manager->m_logic_cache_handles[++m_manager->m_global_index] = handle;
        std::cout << " input logic cache " << m_manager->m_global_index << std::endl;
    }
};
