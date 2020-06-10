/**
 * @author manhelp / https://github.com/manhelp
 */

#ifndef JSP_FINDER_H_
#define JSP_FINDER_H_

#include <malloc.h>
#include <string>
#include <vector>
#include <tuple>
#include <list>
#include <math.h>
#include <string.h>

#define m_max(a,b)            (((a) > (b)) ? (a) : (b))
#define m_min(a,b)            (((a) < (b)) ? (a) : (b))


struct CellData
{
    int x;
    int y;
    bool walkable;
    CellData()
        :x(0), y(0), walkable(false)
    {}
};

struct CellNode
{
    CellData data;

    int f;
    int g;
    int h;
    bool opened;
    bool closed;
    CellNode* parent;
    CellNode()
    {
        clear();
    }

    void clear()
    {
        f = 0;
        g = 0;
        h = 0;
        opened = false;
        closed = false;
        parent = nullptr;
    }
};


class Grid
{
    public:
        friend class JPSFinder;

        Grid();
        ~Grid();

    public:
        CellNode * getcell(int x, int y)
        {
            return &cell_nodes_[y*width_ + x];
        }

        bool isinside(int x, int y)
        {
            return (x >= 0 && x < width_) && (y >= 0 && y < heigth_);
        }

        bool canwalk(int x, int y)
        {
            return isinside(x, y) && getcell(x, y)->data.walkable;
        }

        bool get_neighbors(std::vector<CellNode*>& neighbors, CellNode* cell);

    private:
        CellNode * cell_nodes_;
        int width_;
        int heigth_;
};


class JPSFinder
{
    public:
        struct Point
        {
            int x = 0;
            int y = 0;
        };

    public:
        JPSFinder();
        ~JPSFinder();

        bool init(CellData* cellData, int width, int height);
        std::list<Point> find_path(int sx, int sy, int ex, int ey);

    private:
        void identify_successors(CellNode* node);
        std::tuple<int, int> jump(int x, int y, int px, int py);
        std::vector<CellNode*> find_neighbors(CellNode* cellNode);
        void backtrace(CellNode* node);
        std::list<Point> expandpath();
        std::vector<Point> interpolate(int x0, int y0, int x1, int y1);

    private:
        CellNode * create()
        {
            if ( !free_list_.empty() )
            {
                CellNode* node = free_list_.front();
                free_list_.pop_front();
                node->clear();
                return node;
            }
            return new CellNode();
        }

        void realse(CellNode* node)
        {
            free_list_.push_back(node);
        }

        int manhattan(int x, int y)
        {
            return x + y;
        }

        int octile(int dx, int dy)
        {
            double F = sqrt(2) - 1;
            return (int)(dx < dy ? F * dx + dy : F * dy + dx);
        }

    private:
        Grid grid_;
        std::list<CellNode*> open_list_;
        CellNode* begin_node_;
        CellNode* end_node_;
        std::list<CellNode*> free_list_;
        std::list<Point> path_;
};

#endif //JSP_FINDER_H_
