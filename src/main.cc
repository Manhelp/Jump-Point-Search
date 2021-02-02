#include "TestMap.h"
#include <time.h>
#include "Coroutine.h"
#include "Generator.h"
#include "Manager.h"
#include "Tutorial.h"
#include <chrono>
#include <vector>
#include "Manager.h"

int main(int argc, char* argv[])
{
    // jsp
    /*
    {
        TestMap map;
        std::string filename = "test";
        map.loadcsv(filename);

        int run_times = 100000;
        int dx = 59;
        int dy = 45;
        map.jps_finder_.init(map.cell_datas_, map.width_, map.heigth_);

        clock_t begin_ck = clock();
        printf("run times:%d, start clock:%ld ",run_times, begin_ck);

        std::list<JPSFinder::Point> path = map.jps_finder_.find_path(1, 1, dx, dy);
        for(int i = 0; i < run_times; ++i)
        {
            map.jps_finder_.find_path(1, 1, dx, dy);
        }

        clock_t end_ck = clock();
        printf("end clock:%ld clocks_per_sec:%ld ", end_ck, CLOCKS_PER_SEC);

        printf("total time:%fsec, one time:%fms\n",
                (double)((end_ck - begin_ck)*1.0/CLOCKS_PER_SEC),
                (double)((end_ck - begin_ck)*1.0/CLOCKS_PER_SEC/run_times*1000));

        printf("path size:%d\n", (int)path.size());



        for ( int i = 0; i < map.width_*map.heigth_; ++i )
        {
            if (i % map.width_ == 0 )
                printf("\n");

            int f = map.cell_datas_[i].walkable ? 1 : 0;

            if ( !path.empty() )
            {
                for ( auto itr = path.begin(); itr != path.end(); ++itr )
                {
                    if ( itr->x == map.cell_datas_[i].x && itr->y == map.cell_datas_[i].y )
                    {
                        f = 8;
                        break;
                    }
                }
            }
            if(f == 8) {
                printf(" ");
            } else {
                printf("%d", f);
            }
        }
        std::cout << '\n';
    }
    */

    std::cout << "hardware_concurrency=" << std::thread::hardware_concurrency() << std::endl;

    /*
        // tutorial
        // 创建一个任务
        test_task::ptr_t task = std::make_shared<test_task>(3);
        // 运行任务
        auto fut = g(task);
        // 模拟从外部获取数据然会恢复协程
        while (!fut.done()) {
            test_rpc_manager_run();
        }
    */
    // switch to new thread
    // resuming_on_new_thread();

    // generator
    /*
        for (int i : range(-4, 8)) {
            std::cout << i << ' ';
        }
        std::cout << '\n';
    */

    /*
        A* a = new A();
        B* b = new B();

        AA* aa = new AA();
        BB* bb = new BB();
        CBA* cba = new CBA();

        A* a_a = (A*)aa;
        B* b_b = (B*)bb;
        A* a_ab = (A*)cba;
        B* b_ab = (B*)cba;
        Base* base_ab = (Base*)cba;

        std::cout << "Base len = " << sizeof(Base) << " sizeof(std::size_t)=" << sizeof(std::size_t) << std::endl;
        std::cout << "std::aligned_storage<48>::type = " << sizeof(std::aligned_storage<48>::type) << std::endl;

        if(cba->iscoro()) {
            std::cout << "cba iscoro" << std::endl;
        } else
        {
            std::cout << "cba not iscoro" << std::endl;   
        }

        
        if(base_ab->iscoro()) {
            std::cout << "base iscoro" << std::endl;
        } else
        {
            std::cout << "base not iscoro" << std::endl;   
        }

    */
    	

    

    std::cout << "--------------------------------------------------------------------" << std::endl;
    std::cout << "1" << " thread id " << std::this_thread::get_id() << std::endl;


    thread_manager::instance();
    

    // main thread
    while(true) {


        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    return 0;
};
