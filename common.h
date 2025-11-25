#ifndef SNAKEGAME_COMMON_H
#define SNAKEGAME_COMMON_H
#include <iostream>
#include <array>


struct Point{
    int x;
    int y;

    Point() : x(0), y(0){}
    Point(int x, int y) : x(x), y(y){}

    bool operator==(const Point &other)const{
        return (x == other.x) && (y == other.y);
    }

    bool operator!=(const Point &other)const{
        return (x != other.x) || (y != other.y);
    }

    bool operator<(const Point &other)const{
        return (x < other.x) || (x == other.x && y < other.y);
    }
    Point operator+(const Point &o) const {
        return { x + o.x, y + o.y };
    }
// Für Pfadfindungsalgorithmen, die Point hash müssen
    friend std::ostream& operator<<(std::ostream &os,const Point &p){
        os <<"( "<<p.x <<","<<p.y<<" )";
        return os;
    }
};

// Cell types für das Grid
enum class CellType {
    EMPTY,
    WALL,
    SNAKE_BODY,
    FOOD
};

// Einfach direction enum
enum class Direction {
    UP,
    DOWN,
    LEFT,
    RIGHT,
    NONE
};

// Globale direction Klasse
class Directions {
public:
    // Static direction vectors als Points
    inline static const Point UP    = {  0, -1 };
    inline static const Point DOWN  = {  0,  1 };
    inline static const Point LEFT  = { -1,  0 };
    inline static const Point RIGHT = {  1,  0 };

    // Array von alle 4 main directions fuer einfachen iteration
    inline static const std::array<Point,4> ALL_DIRECTIONS = {
            UP, DOWN, LEFT, RIGHT
    };

};

#endif //SNAKEGAME_COMMON_H
