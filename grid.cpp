#include "grid.h"

Grid::Grid(int breite, int hohe,bool createBorder):breite(breite),hohe(hohe),cells(hohe,std::vector<CellType>(breite,CellType::EMPTY)){
    if(createBorder){
        Grid::initializeWalls();
    }
}
void Grid::initializeWalls() {
    //Horizontale Waende
    for(int x = 0; x < breite; x++){
        setCell(x,0,CellType::WALL);
        setCell(x,hohe-1,CellType::WALL);
    }
    //Vertikale Waende
    for(int y = 0; y < hohe; y++){
        setCell(0,y,CellType::WALL);
        setCell(breite-1,y,CellType::WALL);
    }
}

//Auf Cells zugreifen
CellType Grid::getCell(const Point &p)const{
    return getCell(p.x,p.y);
};
CellType Grid::getCell(int x , int y) const{
    if (!isInBounds(x,y)) {
        return CellType::WALL;
    }
    return cells[y][x];
};
void Grid::setCell(const Point &p, CellType type){
    setCell(p.x,p.y,type);
};
void Grid::setCell(int x,int y,CellType type){
    if (isInBounds(x,y)) {
        cells[y][x] = type;
    }
};


// Schauen ob ein Punkt in den Grenzen liegt
bool Grid::isInBounds(const Point& p)const{
    return isInBounds(p.x,p.y);
};
bool Grid::isInBounds(int x, int y)const{
    return (x >= 0 && x < breite && y >= 0 && y < hohe);
};
bool Grid::isObstacle(const Point &p) const {
    return getCell(p) == CellType::WALL;
}
//setzen von cellen auf leer
void Grid::clear(CellType type){
    for(int y = 0; y < hohe; y++){
        for(int x = 0; x < breite; x++){
            if (cells[y][x] != CellType::WALL){
                cells[y][x] = type;
            }
        }
    }
};

void Grid::setPath(const std::vector<Point>& path) {
    currentPath = path;
}

void Grid::clearPath() {
    currentPath.clear();
}
void Grid::clearObstacles() {
    // Durchlaufe nur die inneren Zellen, also x=1..breite-2, y=1..hohe-2
    for (int y = 1; y < hohe - 1; ++y) {
        for (int x = 1; x < breite - 1; ++x) {
            if (cells[y][x] == CellType::WALL) {
                cells[y][x] = CellType::EMPTY;
            }
        }
    }
}
//printen des grids fuer debug
void Grid::renderToSDL(SDL_Renderer* renderer, int cellSize) const {
    for (int y = 0; y < hohe; ++y) {
        for (int x = 0; x < breite; ++x) {
            SDL_Rect rect = {x * cellSize, y * cellSize, cellSize, cellSize};

            switch (cells[y][x]) {
                case CellType::WALL:
                    SDL_SetRenderDrawColor(renderer, 128, 128, 128, 255); // Grau
                    break;
                case CellType::SNAKE_BODY:
                    SDL_SetRenderDrawColor(renderer, 0, 200, 0, 255); // Dunkel Grüne
                    break;
                case CellType::FOOD:
                    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Rot
                    break;
                default:
                    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Schwarz (Leer)
                    break;
            }

            SDL_RenderFillRect(renderer, &rect);

            if (cells[y][x] != CellType::EMPTY) {
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // Weiße Grenze(border)
                SDL_RenderDrawRect(renderer, &rect);
            }
        }
    }
}
//Separates Pfad-Overlay
void Grid::renderPathOverlay(SDL_Renderer* renderer, int cellSize) const {
    if (currentPath.empty()) return;

    // Pfad mit halbtransparenten Gelb rendern
    SDL_SetRenderDrawColor(renderer, 255, 255, 0, 100); // Gelb mit Transparenz

    for (const Point& pathPoint : currentPath) {
        if (isInBounds(pathPoint)) {
            // Nur rendern wenn die Zelle leer ist (nicht über Snake/Food/Wände)
            if (cells[pathPoint.y][pathPoint.x] == CellType::EMPTY) {
                SDL_Rect rect = {
                        pathPoint.x * cellSize + 2,  // Kleiner Offset für bessere Sicht
                        pathPoint.y * cellSize + 2,
                        cellSize - 4,
                        cellSize - 4
                };
                SDL_RenderFillRect(renderer, &rect);

                // Grenze um Pfad-Zellen
                SDL_SetRenderDrawColor(renderer, 255, 200, 0, 150); // Etwas dunkleres Gelb
                SDL_RenderDrawRect(renderer, &rect);
                SDL_SetRenderDrawColor(renderer, 255, 255, 0, 100); // Zurück zur Hauptfarbe
            }
        }
    }
}