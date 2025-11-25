#ifndef SNAKEGAME_PATHFINDER_H
#define SNAKEGAME_PATHFINDER_H

#include "Snake.h"
#include <functional>
#include <vector>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <memory>

// Funktor zum Berechnen eines Hash-Werts für Objekte vom Typ Point,
// damit sie z.B. in std::unordered_map oder std::unordered_set verwendet werden können.
struct PointHash{
    size_t operator()(const Point &p) const{
        return std::hash<int>()(p.x) ^ (std::hash<int>()(p.y) << 1);
    }
};
class Pathfinder{
public:
    //
    std::vector<Point> getNeighbors(const Point &p, const Grid &grid,const Snake &snake) const;

    std::vector<Direction> pathToDirections(const std::vector<Point> &path, Point startpos) const;

    Direction getDirection(const Point &from,const Point &to)const;

    virtual std::vector<Point> findPath(Point start, Point goal, const Grid &grid,const Snake &snake) = 0;

    virtual ~Pathfinder() = default;

    enum class Algorithm{
        BFS,
        DIJKSTRA
    };

    static std::unique_ptr<Pathfinder> create(Algorithm algo);
};

class BFSPathfinder : public Pathfinder{
    std::vector<Point> findPath(Point start, Point goal, const Grid &grid,const Snake &snake)override;
};

class DijkstraPathfinder : public Pathfinder{
    std::vector<Point> findPath(Point start, Point goal, const Grid &grid,const Snake &snake)override;
};

#endif //SNAKEGAME_PATHFINDER_H
