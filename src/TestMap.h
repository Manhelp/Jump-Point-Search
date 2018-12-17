#include "JPSFinder.h"

class TestMap
{
    public:
        TestMap();
        ~TestMap();
    public:
        int parsecsv(std::vector<std::string>& strings, const char* buf);
        bool loadcsv(std::string filename);
        void stringsplit(std::vector<std::string>&result, const char* regex, const char* pattern);

    public:
        CellData * cell_datas_;
        int width_;
        int heigth_;

        JPSFinder jps_finder_;
};
