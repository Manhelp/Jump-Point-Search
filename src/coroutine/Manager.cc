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
   
    if(index % 1000000 == 0) {
            std::cout << " begin run " << index << " thread id " << std::this_thread::get_id() << std::endl;
        }
    co_await thread_manager::await_suspend_handle(*this);
    // std::cout << " continue run " << index << " thread id " << std::this_thread::get_id() << std::endl;
    auto itor = m_main_handles.find(index);
    if(itor != m_main_handles.end()) {
        if(index % 1000000 == 0) {
            std::cout << " continue run " << index << " result " << itor->second->m_data << " thread id " << std::this_thread::get_id() << std::endl;
        }
    }
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

            for(int i = 0; i < 1000000; ++i) {
                manager->async_task();
            }

        }

        // cosume
        if(!manager->m_main_cache_handles.empty()) {
            {
                std::lock_guard<std::mutex> guard(manager->m_main_mutex);
                manager->m_main_handles.swap(manager->m_main_cache_handles);
            }
            

            for(auto itor = manager->m_main_handles.begin(); itor != manager->m_main_handles.end(); ++itor) {
                // std::cout << " resume " << itor->first << " thread id " << std::this_thread::get_id() << std::endl;
                itor->second->m_handle.resume();
                
                manager->release(itor->second);
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
            {
                std::lock_guard<std::mutex> guard(manager->m_logic_mutex);
                manager->m_logic_handles.swap(manager->m_logic_cache_handles);
            }
            
            for(auto itor = manager->m_logic_handles.begin(); itor != manager->m_logic_handles.end(); ++itor) {
                // sync
                // do something
                // .......
                // std::this_thread::sleep_for(std::chrono::milliseconds(1));
                itor->second->m_data = 888;
                // .......
                // finish

                {
                    std::lock_guard<std::mutex> guard(manager->m_main_mutex);
                    manager->m_main_cache_handles.emplace(std::make_pair(itor->first, itor->second));
                }
                // manager->m_results.emplace(std::make_pair(itor->first, itor->first));
                
                // std::cout << " finish logic operate " << itor->first << " thread id " << std::this_thread::get_id() << std::endl;
            }

            manager->m_logic_handles.clear();
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}