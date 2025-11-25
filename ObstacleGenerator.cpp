#include "ObstacleGenerator.h"

// Hilfsfunktion für Pathfinding
bool ObstacleGenerator::canReachAllAreas(const Point& start) {
    std::vector<std::vector<bool>> visited(hohe, std::vector<bool>(breite, false));
    std::queue<Point> queue;

    queue.push(start);
    visited[start.y][start.x] = true;
    int reachableCells = 1;

    while(!queue.empty()) {
        Point current = queue.front();
        queue.pop();

        for(const Point& dir : Directions::ALL_DIRECTIONS) {
            Point next = {current.x + dir.x, current.y + dir.y};

            if(grid->isInBounds(next) && !visited[next.y][next.x] &&
               !grid->isObstacle(next)) {
                visited[next.y][next.x] = true;
                queue.push(next);
                reachableCells++;
            }
        }
    }

    // Mindestens 50% des verfügbaren Raums sollte erreichbar sein
    int totalEmptySpace = (breite-2) * (hohe-2);
    return reachableCells >= totalEmptySpace * 0.5;
}


ObstacleGenerator::ObstacleGenerator(Grid* g) : grid(g), rng(std::random_device{}()) {
    breite = g->getBreite();
    hohe = g->getHohe();
}

// 1. EINFACHE ZUFÄLLIGE HINDERNISSE
void ObstacleGenerator::generateRandomObstacles(int count) {
    std::uniform_int_distribution<int> distX(2, breite-3);
    std::uniform_int_distribution<int> distY(2, hohe-3);

    int placed = 0;
    int attempts = 0;
    const int maxAttempts = count * 15;

    while(placed < count && attempts < maxAttempts) {
        Point pos = {distX(rng), distY(rng)};

        if(grid->getCell(pos) == CellType::EMPTY) {
            grid->setCell(pos, CellType::WALL);
            placed++;
        }
        attempts++;
    }
}

// 2. RECHTECKIGE BLÖCKE
void ObstacleGenerator::generateBlocks(int count, int minSize, int maxSize) {
    std::uniform_int_distribution<int> distX(2, breite-maxSize-1);
    std::uniform_int_distribution<int> distY(2, hohe-maxSize-1);
    std::uniform_int_distribution<int> distSize(minSize, maxSize);

    for(int i = 0; i < count; i++) {
        Point start = {distX(rng), distY(rng)};
        int width = std::min(distSize(rng), breite - start.x - 2);
        int height = std::min(distSize(rng), hohe - start.y - 2);

        // Prüfe ob Bereich frei ist
        bool canPlace = true;
        for(int y = start.y; y < start.y + height && canPlace; y++) {
            for(int x = start.x; x < start.x + width && canPlace; x++) {
                if(grid->getCell(x, y) != CellType::EMPTY) {
                    canPlace = false;
                }
            }
        }

        // Platziere Block
        if(canPlace) {
            for(int y = start.y; y < start.y + height; y++) {
                for(int x = start.x; x < start.x + width; x++) {
                    grid->setCell(x, y, CellType::WALL);
                }
            }
        }
    }
}

// 3. KREUZ-MUSTER IN DER MITTE
void ObstacleGenerator::generateCenterCross() {
    int centerX = breite / 2;
    int centerY = hohe / 2;
    int armLength = std::min(breite, hohe) / 6;

    // Horizontaler Arm
    for(int x = centerX - armLength; x <= centerX + armLength; x++) {
        if(grid->isInBounds(x, centerY)) {
            grid->setCell(x, centerY, CellType::WALL);
        }
    }

    // Vertikaler Arm
    for(int y = centerY - armLength; y <= centerY + armLength; y++) {
        if(grid->isInBounds(centerX, y)) {
            grid->setCell(centerX, y, CellType::WALL);
        }
    }
}

// 4. ECK-HINDERNISSE
void ObstacleGenerator::generateCornerObstacles() {
    int size = 3;
    std::vector<Point> corners = {
            {2, 2},                    // Oben links
            {breite-2-size, 2},        // Oben rechts
            {2, hohe-2-size},          // Unten links
            {breite-2-size, hohe-2-size}  // Unten rechts
    };

    for(const Point& corner : corners) {
        // L-förmiges Hindernis in jeder Ecke
        for(int i = 0; i < size; i++) {
            grid->setCell(corner.x + i, corner.y, CellType::WALL);
            grid->setCell(corner.x, corner.y + i, CellType::WALL);
        }
    }
}

// 5. Maze-ÄHNLICHE STRUKTUREN
void ObstacleGenerator::generateMazeWalls(float density) {
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);

    for(int y = 3; y < hohe-2; y += 2) {
        for(int x = 3; x < breite-2; x += 2) {
            if(dist(rng) < density) {
                grid->setCell(x, y, CellType::WALL);

                // Zufällige Verbindung in eine Richtung
                std::vector<Point> directions = {{0,1}, {1,0}, {0,-1}, {-1,0}};
                std::shuffle(directions.begin(), directions.end(), rng);

                for(const Point& dir : directions) {
                    Point next = {x + dir.x, y + dir.y};
                    if(grid->isInBounds(next) &&
                       grid->getCell(next) == CellType::EMPTY) {
                        grid->setCell(next, CellType::WALL);
                        break;
                    }
                }
            }
        }
    }
}

// 6. SICHERE HINDERNIS-GENERIERUNG (verhindert unspielbare Level)
void ObstacleGenerator::generateSafeObstacles(const Point& snakeStart, int count) {
    std::uniform_int_distribution<int> distX(2, breite-3);
    std::uniform_int_distribution<int> distY(2, hohe-3);

    int placed = 0;
    int attempts = 0;
    const int maxAttempts = count * 20;

    while(placed < count && attempts < maxAttempts) {
        Point pos = {distX(rng), distY(rng)};

        if(grid->getCell(pos) == CellType::EMPTY) {
            // Temporär platzieren
            grid->setCell(pos, CellType::WALL);

            // Prüfe ob noch genügend Raum erreichbar ist
            if(canReachAllAreas(snakeStart)) {
                placed++;
            } else {
                // Rückgängig machen
                grid->setCell(pos, CellType::EMPTY);
            }
        }
        attempts++;
    }
}

// 7. LEVEL-BASIERTE GENERIERUNG
void ObstacleGenerator::generateForLevel(int level, const Point& snakeStart) {
    switch(level) {
        case 1:
            generateRandomObstacles(3);
            break;
        case 2:
            generateRandomObstacles(5);
            break;
        case 3:
            generateBlocks(2, 2, 3);
            break;
        case 4:
            generateCornerObstacles();
            break;
        case 5:
            generateCenterCross();
            generateRandomObstacles(3);
            break;
        case 6:
            generateMazeWalls(0.2f);
            break;
        default:
            // Für höhere Level: Sichere Generierung mit steigender Schwierigkeit
            generateSafeObstacles(snakeStart, std::min(level, 15));
            break;
    }
}

// 8. CUSTOM MUSTER
[[maybe_unused]] void ObstacleGenerator::generatePattern(const std::vector<Point>& pattern, const Point& offset) {
    for(const Point& p : pattern) {
        Point pos = {offset.x + p.x, offset.y + p.y};
        if(grid->isInBounds(pos)) {
            grid->setCell(pos, CellType::WALL);
        }
    }
}

