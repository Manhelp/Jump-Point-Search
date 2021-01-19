#include "Manager.h"

bool g_start_loop = false;
int64_t thread_manager::m_global_index = 0;
int64_t thread_manager::m_gen_task_tick = 0;

thread_manager::thread_manager() {
    m_main_thread = std::jthread(thread_manager::main_thread_worker, this);
    m_main_thread.detach();

    m_logic_thread = std::jthread(thread_manager::logic_thread_worker, this);
    m_logic_thread.detach();

    g_start_loop = true;
}


jthread_awaitable startCoroutine(async_func func)
{
    result_data* data = thread_manager::instance()->alloc(thread_manager::m_global_index);
    data->m_async_func = func;
    return jthread_awaitable{thread_manager::instance(), data};
}

void setValue(uint value)
{
    std::cout << " setValue " << " thread id " << std::this_thread::get_id() << std::endl;
}

void addValue(uint add)
{
    std::cout << " addValue " << " thread id " << std::this_thread::get_id() << std::endl;
}

void doSomeThing()
{
    std::cout << " doSomeThing " << " thread id " << std::this_thread::get_id() << std::endl;
}

void loadDataFromMysql()
{
   std::cout << " loadDataFromMysql "  << " thread id " << std::this_thread::get_id() << std::endl;
}


coroutine_task update()
{
    std::cout << " startCoroutine(loadDataFromMysql) "  << " thread id " << std::this_thread::get_id() << std::endl;
    co_await startCoroutine(loadDataFromMysql);

    setValue(1000);

    std::cout << " startCoroutine(doSomeThing) "  << " thread id " << std::this_thread::get_id() << std::endl;
    
    co_await startCoroutine(doSomeThing);

    addValue(100);
}

coroutine_task thread_manager::async_task() {
    // generator
    auto index = thread_manager::m_global_index;
   
    if(index % 1000000 == 0) {
        std::cout << " begin run " << index << " thread id " << std::this_thread::get_id() << std::endl;
    }
    result_data* data = thread_manager::instance()->alloc(index);
    co_await thread_manager::await_suspend_handle(this, data);
    // std::cout << " continue run " << index << " thread id " << std::this_thread::get_id() << std::endl;
    auto itor = m_main_handles.find(index);
    if(itor != m_main_handles.end()) {
        if(index % 1000000 == 0) {
            std::cout << " continue run " << index << " result " << itor->second->m_data << " thread id " << std::this_thread::get_id() << std::endl;
        }
    }
}

jthread_awaitable thread_manager::await_suspend_handle(thread_manager* manager, result_data* data) {
    return jthread_awaitable{manager, data};
}

void thread_manager::main_thread_worker(thread_manager* manager) {
    while (true)
    {
        if(!g_start_loop) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            continue;
        }
        // genertor
        if(thread_manager::m_gen_task_tick < time(nullptr)) {
            thread_manager::m_gen_task_tick = time(nullptr) + 10000000;
            
            

            update();

            // for(int i = 0; i < 1; ++i) {
                // manager->async_task();
            // }

        }

        

        // cosume
        if(!manager->m_main_cache_handles.empty()) {
            {
                std::lock_guard<std::mutex> guard(manager->m_main_mutex);
                manager->m_main_handles.swap(manager->m_main_cache_handles);
            }
            

            for(auto itor = manager->m_main_handles.begin(); itor != manager->m_main_handles.end(); ++itor) {

                std::cout << " resume " << itor->first << " thread id " << std::this_thread::get_id() << std::endl;
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
        if(!g_start_loop) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            continue;
        }
        
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
                itor->second->m_async_func();
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