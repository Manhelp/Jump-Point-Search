#include "TestMap.h"

TestMap::TestMap()
    :cell_datas_(nullptr)
    , width_(0)
     , heigth_(0)
{

}

TestMap::~TestMap()
{
    if ( cell_datas_ )
        delete[] cell_datas_;
}

int TestMap::parsecsv(std::vector<std::string>& strings, const char* buf)
{
    if ( buf == nullptr )
        return 0;

    strings.clear();

    const char* start = nullptr;
    std::string tmpstr;
    const size_t length = strlen(buf);

    for ( size_t i = 0; i <= length; i++ )
    {
        if ( buf[i] == ',' || buf[i] == '\r' ||
                buf[i] == '\n' || buf[i] == '\0' )
        {
            if ( start != nullptr )
            {
                tmpstr.assign(start, &buf[i]);
                strings.push_back(tmpstr);
                start = nullptr;
            }
            else
            {
                tmpstr = "0";
                strings.push_back(tmpstr);
            }

            if ( buf[i] != ',' )
                break;
        }
        else
        {
            if ( start == nullptr )
                start = &buf[i];
        }
    }

    return (int)strings.size();
}

bool TestMap::loadcsv(std::string filename)
{
    std::string path("./");
    path.append(filename);
    path.append(".csv");

    FILE* pf = fopen(path.c_str(), "rb");
    if ( !pf )
    {
        return false;
    }

    char line[20480] = { 0 };
    std::vector<std::string> tmpstr;
    int y = 0;
    while ( fgets(line, sizeof(line), pf) )
    {
        if ( 0 == strlen(line) )
            continue;

        tmpstr.clear();

        if ( line[0] == '@' )
        {
            stringsplit(tmpstr, line, "=");
            if ( tmpstr.size() == 2 )
            {
                if ( strcmp(line, "@width") == 0 )
                {
                    width_ = atoi(tmpstr[1].c_str());
                }
                else if ( strcmp(tmpstr[0].c_str(), "@heigth") == 0 )
                {
                    heigth_ = atoi(tmpstr[1].c_str());
                }

                if ( !cell_datas_ && width_ > 0 && heigth_ > 0 )
                {
                    cell_datas_ = new CellData[width_*heigth_];
                }
            }
        }
        else
        {
            if ( parsecsv(tmpstr, line) > 0 )
            {
                int x = 0;
                for ( auto var : tmpstr )
                {
                    cell_datas_[y*width_ + x].x = x;
                    cell_datas_[y*width_ + x].y = y;
                    if ( atoi(var.c_str()) != 0 )
                        cell_datas_[y*width_ + x].walkable = true;

                    ++x;
                }

                ++y;
            }
        }

    }


    return true;
}

void TestMap::stringsplit(std::vector<std::string>&result, const char* regex, const char* pattern)
{
    if ( !strstr(regex, pattern) )
    {
        result.push_back(regex);
        return;
    }
    regex = strtok((char*)regex, pattern);
    while ( regex != nullptr )
    {
        result.push_back(regex);
        regex = strtok(nullptr, pattern);
    }
    return;
}




