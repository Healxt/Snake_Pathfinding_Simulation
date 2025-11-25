#include "Snake.h"

Snake::Snake(Point startPosition, Direction startDirection) : currentDirection(startDirection), shouldGrow(false) {
    body.clear();
    body.push_back(startPosition);
}
void Snake::move(){
    //GET Kopf aktulle Position
    Point newhead = getHeadPosition();

    //UPDATE Kopf Position nach der Richtung
    switch (currentDirection){
        case Direction::UP:
            newhead.y--;
            break;
        case Direction::DOWN:
            newhead.y++;
            break;
        case Direction::LEFT:
            newhead.x--;
            break;
        case Direction::RIGHT:
            newhead.x++;
            break;
        case Direction::NONE:
        //
            return;
    }
    //ADD Kopf Position in den Body
    body.push_front(newhead);

    //REMOVE Snake schwanz(letzten Kopf Position) aus dem Body
    if (!shouldGrow){
        body.pop_back();
    }else{
        shouldGrow = false;
    }
}

void Snake::grow(){
    shouldGrow = true;
}

void Snake::setDirection(Direction newdir){
    //verhindern, dass sich der Snake in einer Richtung drehen kann, in der er sich selbst bewegt, also(180 grad Rotation verhindern)
    if (currentDirection == Direction::UP && newdir == Direction::DOWN
    || currentDirection == Direction::DOWN && newdir == Direction::UP
    || currentDirection == Direction::LEFT && newdir == Direction::RIGHT
    || currentDirection == Direction::RIGHT && newdir == Direction::LEFT){
        return;
    }
    currentDirection = newdir;
}

bool Snake::checkWallCollision(const Grid &grid) const{
    Point head = getHeadPosition();
    return grid.getCell(head) == CellType::WALL;
}

bool Snake::checkSelfCollision() const{
    if (body.size() <= 1){
        return false;
    }

    Point head = getHeadPosition();
    auto it = body.begin();
        ++it;
    //itrieren durch den Body und pruefen, ob der Kopf die gleiche Point ist wie ein Bodyteil
    while(it != body.end()){
        if (*it == head){
            return true;
        }
        ++it;
    }
    return false;
}

//render die Snake
void Snake::draw(Grid &grid) const{
    for (const Point &currentBodyPoint : body){
        grid.setCell(currentBodyPoint,CellType::SNAKE_BODY);
    }
}