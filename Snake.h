#pragma once
#include <deque>
#include "common.h"
#include "grid.h"

class Snake{
private:
    std::deque<Point> body;
    Direction currentDirection;
    bool shouldGrow;
public:
    Snake(Point startPosition, Direction startDirection = Direction::RIGHT);

    // Bewegung und Kontrolle
    void move();
    void grow();
    void setDirection(Direction dir);

    // Status getters
    [[nodiscard]] Point getHeadPosition() const{return body.front();};
    [[nodiscard]] const std::deque<Point>& getBody() const{return body;};
    [[nodiscard]] Direction getDirection() const{return currentDirection;};

    //Kollisionen pruefen (wand oder selbst)
    [[nodiscard]] bool checkWallCollision(const Grid &grid) const;
    [[nodiscard]] bool checkSelfCollision() const;


    //rendert die Snake
    void draw(Grid &grid) const;

};
