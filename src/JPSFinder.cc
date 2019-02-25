/**
 * @author manhelp / https://github.com/manhelp
 */

#include "JPSFinder.h"
#include "TestMap.h"

Grid::Grid()
    :cell_nodes_(nullptr)
    , width_(0)
     , heigth_(0)
{
}
Grid::~Grid()
{
    if ( cell_nodes_ )
        delete[] cell_nodes_;
}



bool Grid::get_neighbors(std::vector<CellNode*>& neighbors, CellNode* cell)
{
    int x = cell->data.x;
    int y = cell->data.y;
    if ( canwalk(x, y - 1) )//up
        neighbors.push_back(getcell(x, y - 1));

    if ( canwalk(x + 1, y) )//right
        neighbors.push_back(getcell(x + 1, y));

    if ( canwalk(x, y + 1) )//down
        neighbors.push_back(getcell(x, y + 1));

    if ( canwalk(x - 1, y) )//left
        neighbors.push_back(getcell(x - 1, y));

    if ( canwalk(x - 1, y - 1) )//left up
        neighbors.push_back(getcell(x - 1, y - 1));

    if ( canwalk(x + 1, y - 1) )//right up
        neighbors.push_back(getcell(x + 1, y - 1));

    if ( canwalk(x + 1, y + 1) )//right down
        neighbors.push_back(getcell(x + 1, y + 1));

    if ( canwalk(x - 1, y + 1) )//left down
        neighbors.push_back(getcell(x - 1, y + 1));

    return true;
}

JPSFinder::JPSFinder()
{
}

JPSFinder::~JPSFinder()
{
}

bool JPSFinder::init(TestMap* map)
{
    if(!map || !map->cell_datas_ || map->width_ == 0 || map->heigth_ == 0)
        return false;

    grid_.width_ = map->width_;
    grid_.heigth_ = map->heigth_;
    grid_.cell_nodes_ = new CellNode[grid_.width_*grid_.heigth_];

    for ( int i = 0; i < grid_.width_*grid_.heigth_; ++i )
    {
        grid_.cell_nodes_[i].data = map->cell_datas_[i];
    }

    return true;
}

std::list<JPSFinder::Point> JPSFinder::find_path(int sx, int sy, int ex, int ey)
{
    for ( int i = 0; i < grid_.width_*grid_.heigth_; ++i )
    {
        grid_.cell_nodes_->clear();
    }
    path_.clear();

    if ( !grid_.isinside(sx, sy) || !grid_.isinside(ex, ey) )
        return expandpath();;

    begin_node_ = grid_.getcell(sx, sy);
    end_node_ = grid_.getcell(ex, ey);

    // set the `g` and `f` value of the start node to be zero
    begin_node_->g = 0;
    begin_node_->f = 0;

    // push the begin node into the open list
    open_list_.push_back(begin_node_);
    begin_node_->opened = true;

    CellNode* node = nullptr;
    while ( !open_list_.empty() )
    {
        // pop the position of node which has the minimum `f` value.
        node = open_list_.front();
        open_list_.pop_front();
        node->closed = true;
        if ( node == end_node_ )
        {
            backtrace(end_node_);
            return expandpath();
        }

        identify_successors(node);
    }
    return expandpath();
}

void JPSFinder::identify_successors(CellNode* node)
{

    CellNode* jumpnode = nullptr;
    int d = 0;
    int g = 0;

    std::vector<CellNode*> cells = find_neighbors(node);
    for ( auto itr = cells.begin(); itr != cells.end(); ++itr )
    {
        auto result = jump((*itr)->data.x, (*itr)->data.y, node->data.x, node->data.y);

        int jx = std::get<0>(result);
        int jy = std::get<1>(result);
        if ( jx < 0 )
            continue;

        jumpnode = grid_.getcell(jx, jy);
        if ( jumpnode->closed )
            continue;

        // include distance, as parent may not be immediately adjacent:
        d = octile(abs(jx - node->data.x), abs(jy - node->data.y));
        g = node->g + d;

        if ( !jumpnode->opened || g < jumpnode->g )
        {
            jumpnode->g = g;
            if ( jumpnode->h == 0 )
                jumpnode->h = manhattan(abs(jx - end_node_->data.x), abs(jy - end_node_->data.y));
            jumpnode->f = jumpnode->g + jumpnode->h;
            jumpnode->parent = node;

            if ( !jumpnode->opened )
            {
                open_list_.push_back(jumpnode);
                open_list_.sort([](const CellNode* a, const CellNode* b)->bool
                        {
                        return a->f < b->f;
                        });

                jumpnode->opened = true;
            }
        }
    }
}

std::tuple<int, int> JPSFinder::jump(int x, int y, int px, int py)
{
    int dx = x - px;
    int dy = y - py;

    if ( !grid_.canwalk(x, y) )
        return std::make_tuple(-1, -1);

    if ( grid_.getcell(x, y) == end_node_ )
        return std::make_tuple(x, y);

    // check for forced neighbors
    // along the diagonal
    if ( dx != 0 && dy != 0 )
    {
        if ( (grid_.canwalk(x - dx, y + dy) && !grid_.canwalk(x - dx, y))
                || (grid_.canwalk(x + dx, y - dy) && !grid_.canwalk(x, y - dy)) )
        {
            return std::make_tuple(x, y);
        }

        if ( std::get<0>(jump(x + dx, y, x, y)) != -1 || std::get<0>(jump(x, y + dy, x, y)) != -1 )
            return std::make_tuple(x, y);
    }
    // horizontally/vertically
    else
    {
        if ( dx != 0 )
        {
            if ( (grid_.canwalk(x + dx, y + 1) && !grid_.canwalk(x, y + 1))
                    || (grid_.canwalk(x + dx, y - 1) && !grid_.canwalk(x, y - 1)) )
            {
                return std::make_tuple(x, y);
            }
        }
        else
        {
            if ( (grid_.canwalk(x + 1, y + dy) && !grid_.canwalk(x + 1, y))
                    || (grid_.canwalk(x - 1, y + dy) && !grid_.canwalk(x - 1, y)) )
            {
                return std::make_tuple(x, y);
            }
        }
    }

    return jump(x + dx, y + dy, x, y);
}

std::vector<CellNode*> JPSFinder::find_neighbors(CellNode* cellNode)
{
    std::vector<CellNode*> neighbors;

    // directed pruning: can ignore most neighbors, unless forced.
    if ( cellNode->parent )
    {
        int x = cellNode->data.x;
        int y = cellNode->data.y;
        // get the normalized direction of travel
        int dx = (x - cellNode->parent->data.x) / m_max(abs(x - cellNode->parent->data.x), 1);
        int dy = (y - cellNode->parent->data.y) / m_max(abs(y - cellNode->parent->data.y), 1);

        // search diagonally
        if ( dx != 0 && dy != 0 )
        {
            if ( grid_.canwalk(x, y + dy) )//down
                neighbors.push_back(grid_.getcell(x, y + dy));

            if ( grid_.canwalk(x + dx, y) )//right
                neighbors.push_back(grid_.getcell(x + dx, y));

            if ( grid_.canwalk(x + dx, y + dy) )//right down 
                neighbors.push_back(grid_.getcell(x + dx, y + dy));

            if ( !grid_.canwalk(x - dx, y) )//0 1
                neighbors.push_back(grid_.getcell(x - dx, y + dy));

            if ( !grid_.canwalk(x, y - dy) )// 1 0
                neighbors.push_back(grid_.getcell(x + dx, y - dy));
        }
        // search horizontally/vertically
        else
        {
            if ( dx == 0 )
            {
                if ( grid_.canwalk(x, y + dy) )
                    neighbors.push_back(grid_.getcell(x, y + dy));

                if ( !grid_.canwalk(x + 1, y) )
                    neighbors.push_back(grid_.getcell(x + 1, y + dy));

                if ( !grid_.canwalk(x - 1, y) )
                    neighbors.push_back(grid_.getcell(x - 1, y + dy));
            }
            else
            {
                if ( grid_.canwalk(x + dx, y) )
                    neighbors.push_back(grid_.getcell(x + dx, y));

                if ( !grid_.canwalk(x, y + 1) )
                    neighbors.push_back(grid_.getcell(x + dx, y + 1));

                if ( !grid_.canwalk(x, y - 1) )
                    neighbors.push_back(grid_.getcell(x + dx, y - 1));
            }
        }
    }
    else
    {
        grid_.get_neighbors(neighbors, cellNode);
    }

    return neighbors;
}

void JPSFinder::backtrace(CellNode* node)
{
    Point point;

    do 
    {
        point.x = node->data.x;
        point.y = node->data.y;
        path_.push_back(point);

        node = node->parent;
    } while (node->parent);

    path_.reverse();
}

std::list<JPSFinder::Point> JPSFinder::expandpath()
{
    std::list<Point> lastpath;

    int len = (int)path_.size();

    if ( len < 2 )
        return lastpath;

    auto itr = path_.begin();
    auto itrnext = ++path_.begin();
    int inc = 0;
    while ( inc < len - 1 )
    {
        std::vector<Point> points = interpolate(itr->x, itr->y, itrnext->x, itrnext->y);
        int size = (int)points.size();
        for ( int i = 0; i < size - 1; ++i )
        {
            lastpath.push_back(points[i]);
        }

        ++itr;
        ++itrnext;
        ++inc;
    }

    lastpath.push_back(*itr);

    return lastpath;
}

std::vector<JPSFinder::Point> JPSFinder::interpolate(int x0, int y0, int x1, int y1)
{
    std::vector<Point> lines;

    int dx = abs(x1 - x0);
    int dy = abs(y1 - y0);

    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;

    int err = dx - dy;
    int e2 = 0;

    Point point;

    while ( true )
    {
        point.x = x0;
        point.y = y0;
        lines.push_back(point);

        if ( x0 == x1 && y0 == y1 )
            break;

        e2 = 2 * err;
        if ( e2 > -dy )
        {
            err -= dy;
            x0 += sx;
        }
        if ( e2 < dx )
        {
            err += dx;
            y0 += sy;
        }
    }

    return lines;
}
