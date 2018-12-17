#include "TestMap.h"
#include <time.h>

int main(int argc, char* argv[])
{

    TestMap map;
    std::string filename = "test";
    map.loadcsv(filename);

    int run_times = 100000;
    int dx = 59;
    int dy = 45;
    map.jps_finder_.init(&map);

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

        printf("%d", f);
    }

    getchar();
    return 0;
};
