#include "Manager.h"



thread_manager::thread_manager() {
    m_main_thread = std::jthread(thread_manager::main_thread_worker, this);
    m_main_thread.detach();

    m_logic_thread = std::jthread(thread_manager::logic_thread_worker, this);
    m_logic_thread.detach();
}

coroutine_task thread_manager::async_task() {
    // generator
    auto index = m_global_index;
    std::cout << " begin run " << index << " thread id " << std::this_thread::get_id() << std::endl;
    co_await thread_manager::await_suspend_handle(*this);
    std::cout << " continue run " << index << " thread id " << std::this_thread::get_id() << std::endl;
}

jthread_awaitable thread_manager::await_suspend_handle(thread_manager& manager) {
    return jthread_awaitable{&manager};
}

void thread_manager::main_thread_worker(thread_manager* manager) {
    while (true)
    {
        // genertor
        if(manager->m_gen_task_tick < time(nullptr)) {
            manager->m_gen_task_tick = time(nullptr) + 1;
            manager->async_task();
        }

        // cosume
        if(!manager->m_main_cache_handles.empty()) {
            do {
                std::lock_guard<std::mutex> guard(manager->m_main_mutex);
                manager->m_main_handles.swap(manager->m_main_cache_handles);
            }while(false);

            for(auto itor = manager->m_main_handles.begin(); itor != manager->m_main_handles.end(); ++itor) {
                std::cout << " resume " << itor->first << " thread id " << std::this_thread::get_id() << std::endl;
                itor->second.resume();
            }
            manager->m_main_handles.clear();
        }
            

        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    
}

void thread_manager::logic_thread_worker(thread_manager* manager) {
    while (true)
    {
        // logic
        if(!manager->m_logic_cache_handles.empty()) {
            do {
                std::lock_guard<std::mutex> guard(manager->m_logic_mutex);
                manager->m_logic_handles.swap(manager->m_logic_cache_handles);
            }while(false);
            
            for(auto itor = manager->m_logic_handles.begin(); itor != manager->m_logic_handles.end(); ++itor) {
                // sync
                // do something
                // .......
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
                // finish

                do {
                    std::lock_guard<std::mutex> guard(manager->m_main_mutex);
                    manager->m_main_cache_handles[itor->first] = itor->second;
                }while(false);
                std::cout << " finish logic operate " << itor->first << " thread id " << std::this_thread::get_id() << std::endl;
            }

            manager->m_logic_handles.clear();
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}