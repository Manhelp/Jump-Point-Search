#pragma once

#include <coroutine>
#include <iostream>
#include <stdexcept>
#include <thread>
#include "Coroutine.h"
#include <unordered_map>
#include <mutex>
#include <memory>
#include <list>

struct jthread_awaitable;

struct result_data {
    int64_t m_index;
    int64_t m_data;
    std::coroutine_handle<> m_handle;
    
    result_data() = default;
    result_data(int64_t index, std::coroutine_handle<> handle) : m_index{index}, m_handle{handle} {}
};

class thread_manager {
public:
    thread_manager();
    coroutine_task async_task();
    result_data* alloc(int64_t index, std::coroutine_handle<> handle) {
        if(!m_result_pool.empty()) {
            result_data* data = m_result_pool.front();
            m_result_pool.pop_front();
            data->m_index = index;
            data->m_handle = handle;
            return data;
        }
        return new result_data(index, handle);
    }
    void release(result_data* data) {
        m_result_pool.push_back(data);
    }
public:
    static void main_thread_worker(thread_manager* manager);
    static void logic_thread_worker(thread_manager* manager);
    static jthread_awaitable await_suspend_handle(thread_manager& manager);
public:
    int64_t m_global_index = 0;
    int64_t m_gen_task_tick = 0;

    std::jthread m_main_thread;
    std::jthread m_logic_thread;

    std::mutex m_main_mutex;
    std::mutex m_logic_mutex;


    std::list<result_data*> m_result_pool;
    /**
     * main thread write to m_logic_cache_handles
     * logic thread swap m_logic_cache_handles to m_logic_handles
     * logic thread write to m_main_cache_handles
     * main thread swap m_main_cache_handles to m_main_handles
     */
    std::unordered_map<int64_t, result_data*> m_main_handles;
    std::unordered_map<int64_t, result_data*> m_main_cache_handles;
    std::unordered_map<int64_t, result_data*> m_logic_handles;
    std::unordered_map<int64_t, result_data*> m_logic_cache_handles;

    // main thread read <---> logic thread write
    std::unordered_map<int64_t, int64_t> m_results;
};

struct jthread_awaitable : public coroutine_awaitable {
    thread_manager* m_manager = nullptr;

    jthread_awaitable(thread_manager* manager) : m_manager{manager} {}
    void await_suspend(std::coroutine_handle<> handle) {

        result_data* data = m_manager->alloc(m_manager->m_global_index, handle);
        
        {
            std::lock_guard<std::mutex> guard(m_manager->m_logic_mutex);
            m_manager->m_logic_cache_handles.emplace(std::make_pair(data->m_index, data));
        }
        

        // std::cout << " input logic cache " << m_manager->m_global_index << " thread id " << std::this_thread::get_id() << std::endl;

        ++m_manager->m_global_index;
    }
};
