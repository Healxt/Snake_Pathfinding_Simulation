#ifndef SNAKEGAME_GRID_H
#define SNAKEGAME_GRID_H
#include <vector>
#include "common.h"
#include <SDL.h>

class Grid{
    private:
        int breite;
        int hohe;
        std::vector<std::vector<CellType>> cells;
        std::vector<Point> currentPath;

    public:
        Grid(int breite, int hohe,bool creatborder = true);

        //Getters
        [[nodiscard]] int getBreite() const { return breite; }
        [[nodiscard]] int getHohe() const { return hohe; }

        //Auf Cells zugreifen
        [[nodiscard]] CellType getCell( const Point &p) const;
        [[nodiscard]] CellType getCell( int x , int y) const;
        void setCell(const Point &p, CellType type);
        void setCell(int x,int y,CellType type);


        // Schauen ob ein Punkt in den Grenzen liegt
        [[nodiscard]] bool isInBounds(const Point& p)const;
        [[nodiscard]] bool isInBounds(int x, int y)const;

        [[nodiscard]] bool isObstacle(const Point &p)const;
        //initialize waende
        void initializeWalls();
        //setzen von cellen auf leer
        void clear(CellType type = CellType::EMPTY);

        // Pfad-Management
        void setPath(const std::vector<Point>& path);
        void clearPath();

        void clearObstacles();

        //printen des grids
        void renderToSDL(SDL_Renderer* renderer, int cellSize) const;
        void renderPathOverlay(SDL_Renderer* renderer, int cellSize)const;
};
#endif //SNAKEGAME_GRID_H
