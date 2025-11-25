#ifndef SNAKEGAME_OBSTACLEGENERATOR_H
#define SNAKEGAME_OBSTACLEGENERATOR_H

#include "grid.h"
#include <random>
#include <vector>
#include <queue>
#include <algorithm>

class ObstacleGenerator {
private:
    std::mt19937 rng;
    Grid* grid;
    int breite, hohe;

public:
    ObstacleGenerator(Grid* g);

    bool canReachAllAreas(const Point& start);
    void generateRandomObstacles(int count);
    void generateBlocks(int count, int minSize = 2, int maxSize = 4);
    void generateCenterCross();
    void generateCornerObstacles();
    void generateMazeWalls(float density = 0.25f);
    void generateSafeObstacles(const Point& snakeStart, int count);
    void generateForLevel(int level, const Point& snakeStart);
    [[maybe_unused]] void generatePattern(const std::vector<Point>& pattern, const Point& offset);

    // Vordefinierte Muster
    [[maybe_unused]] std::vector<Point> getXPattern() {
        return {{0,0}, {1,1}, {2,2}, {2,0}, {1,1}, {0,2}};
    }

    [[maybe_unused]] std::vector<Point> getHPattern() {
        return {{0,0}, {0,1}, {0,2}, {1,1}, {2,0}, {2,1}, {2,2}};
    }
};
#endif //SNAKEGAME_OBSTACLEGENERATOR_H
