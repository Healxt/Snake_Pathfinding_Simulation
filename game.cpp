#include "game.h"

// Game Implementation
Game::Game(int breite, int hohe, int windowW, int windowH)
        : grid(breite, hohe, true),
          snake(Point(breite / 2, hohe / 2), Direction::RIGHT),
          score(0),
          state(GameState::RUNNING),
          rng(std::chrono::steady_clock::now().time_since_epoch().count()),
          window(nullptr),
          renderer(nullptr),
          windowWidth(windowW),
          windowHeight(windowH),
          lastMoveTime(0),
          moveDelay(150), // Move jede 150ms
          obstacleGenerator(&grid),
          currentAlgorithm(Pathfinder::Algorithm::BFS) // Standard
{
    cellSize = std::min(windowWidth / breite, windowHeight / hohe);

    pathfinder = Pathfinder::create(currentAlgorithm);

    generateObstaclesForLevel();

    // Essen spawnen
    for (int i = 0; i < maxFoodItems; ++i) {
        spawnFood();
    }
}

Game::~Game() {
    cleanup();
}

bool Game::initSDL() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }

    window = SDL_CreateWindow("SDL Snake Game",
                              SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                              windowWidth, windowHeight,
                              SDL_WINDOW_SHOWN);

    if (!window) {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }
    // Initialisieren TTF
    if (TTF_Init() == -1) {
        std::cerr << "TTF_Init: " << TTF_GetError() << std::endl;
        return false;
    }
    // Initialisieren HUD: Geben Sie den fontPath relativ zur ausführbaren Datei an
    const std::string fontPath = "K:\\SnakeGame\\Roboto.ttf";
    int fontSize = 12;
    if (!hud.init(renderer, fontPath, fontSize)) {
        std::cerr << "Failed to initialize HUD\n";
        // Falls font nicht funktioiert bekommt man diese Error
    }
    return true;
}

void Game::cleanup() {
    hud.cleanup();

    if (renderer) {
        SDL_DestroyRenderer(renderer);
        renderer = nullptr;
    }
    if (window) {
        SDL_DestroyWindow(window);
        window = nullptr;
    }
    TTF_Quit();
    SDL_Quit();
}

void Game::spawnFood() {
    //spawn nicht mehr als max Anzahl an Essen
    if (foodPositions.size() >= maxFoodItems) {
        return;
    }

    //finde leere cell fuer Essen
    std::uniform_int_distribution<int> distx(1, grid.getBreite() - 2);
    std::uniform_int_distribution<int> disty(1, grid.getHohe() - 2);

    bool found = false;

    Point kandidat;
    while (!found) {
        kandidat = Point(distx(rng), disty(rng));

        //PRUEFEN, ob Essen in leere cellen ist
        if (grid.getCell(kandidat) == CellType::EMPTY) {
            found = true;

            //PRUEFEN, ob den cellen sind Schlange Korper ist
            for (const Point &bodyPos: snake.getBody()) {
                if (bodyPos == kandidat) {
                    found = false;
                    break;
                }
            }
            //PRUEFEN, ob den cellen ist schon mit Essen besetzt (also nicht 2-mal in der gleichen cell spawnen)
            for (const Point &foodPos: foodPositions) {
                if (foodPos == kandidat) {
                    found = false;
                    break;
                }
            }
        }
    }
    //FUEGEN neue Essen Position EIN
    foodPositions.push_back(kandidat);
    grid.setCell(kandidat, CellType::FOOD);
}

void Game::removeFood(const Point& foodPos) {
    for (auto it = foodPositions.begin(); it != foodPositions.end(); ++it) {
        if (*it == foodPos) {
            grid.setCell(foodPos,CellType::EMPTY);
            foodPositions.erase(it);
            break;
        }
    }
}

void Game::processInput(Direction dir) {
    if (state == GameState::RUNNING) {
        snake.setDirection(dir);
    }
}
void Game::generateObstaclesForLevel() {
    Point start = Point(grid.getBreite() / 2, grid.getHohe() / 2);
    obstacleGenerator.generateForLevel(level, start);
}
void Game::update() {
    if (state != GameState::RUNNING) {
        return;
    }

    Uint32 currentTime = SDL_GetTicks();
    if (currentTime - lastMoveTime < moveDelay) {
        return; //Noch ist es nicht Zeit, zubewegen
    }
    lastMoveTime = currentTime;

    // AI-Bewegung berechnen, wenn AutoPlay aktiviert ist
    if (autoPlay) {
        updatePathfinding();
        Direction nextMove = getNextAIMove();
        if (nextMove != Direction::NONE) {
            snake.setDirection(nextMove);
        }
    }

    snake.move();

    if (snake.checkWallCollision(grid)){
        state = GameState::GAME_OVER;
        return;
    }

    if (snake.checkSelfCollision()){
        state = GameState::GAME_OVER;
        return;
    }

    Point head = snake.getHeadPosition();
    bool foodeaten = false;
    for (const Point &foodPos: foodPositions) {
        if (head == foodPos) {
            //vergrößere die Schlange
            snake.grow();

            score++;
            hud.updateScore(score, renderer, windowWidth);

            //entfernen das Essen
            removeFood(foodPos);

            //spawn neues Essen
            spawnFood();

            foodeaten = true;
            break;
        }
    }
    // Neuen Pfad berechnen nach Food-Aufnahme

    if (foodeaten && autoPlay) {
        currentPath.clear();
        currentPathPoints.clear();
        grid.clearPath(); // Grid-Pfad auch löschen
    }
    // Nach Essen: Level-Check
    // Beispiel: alle 5 Punkte neues Level
    if (score > 0 && score % 5 == 0) {
        int newLevel = score / 5 + 1;
        if (newLevel > level) {
            level = newLevel;
            grid.clear();
            grid.clearObstacles();
            generateObstaclesForLevel();
         }
    }
}

void Game::render() {
    // Clear screen
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    // Clear das Grid (außer Wände)
    grid.clear();

    // platziere alle food items
    for (const auto& foodPos : foodPositions) {
        grid.setCell(foodPos, CellType::FOOD);
    }

    // Zeichne die snake
    snake.draw(grid);

    // Render das Grid
    grid.renderToSDL(renderer, cellSize);

    // Pfad rendern (falls vorhanden und AutoPlay aktiviert)
    if (autoPlay) {
        grid.renderPathOverlay(renderer,cellSize);
    }

    // Präsentieren den rendered frame
    hud.render(renderer, windowWidth, windowHeight);
    SDL_RenderPresent(renderer);
}

void Game::updatePathfinding() {
    // Wenn kein Food vorhanden, leere Pfad-Daten und return
    if (foodPositions.empty()) {
        currentPath.clear();
        currentPathPoints.clear();
        grid.clearPath();
        return;
    }

    Point start = snake.getHeadPosition();
    Point goal = findClosestFood(start);

    // DEBUG: Ausgabe der Positionen (0 UP,1 DOWN, 2 LEFT, 3 RIGHT, 4 NONE)
    std::cout << "DEBUG: Snake Head: (" << start.x << "," << start.y << ")" << std::endl;
    std::cout << "DEBUG: Target Food: (" << goal.x << "," << goal.y << ")" << std::endl;
    std::cout << "DEBUG: Snake Direction: " << (int)snake.getDirection() << std::endl;

    if (goal.x == -1 && goal.y == -1) {
        currentPath.clear();
        currentPathPoints.clear();
        grid.clearPath();
        return;
    }

    // Berechne neuen Pfad
    std::vector<Point> pathPoints = pathfinder->findPath(start, goal, grid, snake);

    // DEBUG: Pfad-Informationen
    std::cout << "DEBUG: Path points found: " << pathPoints.size() << std::endl;
    if (!pathPoints.empty()) {
        std::cout << "DEBUG: First path point: (" << pathPoints[0].x << "," << pathPoints[0].y << ")" << std::endl;
    }

    if (!pathPoints.empty()) {
        currentPath = pathfinder->pathToDirections(pathPoints, start);
        currentPathPoints = pathPoints;
        grid.setPath(currentPathPoints);

        // DEBUG: Richtungen ausgeben
        std::cout << "DEBUG: Path directions: ";
        for (Direction dir : currentPath) {
            std::cout << (int)dir << " ";
        }
        std::cout << std::endl;
    } else {
        // Kein Pfad gefunden
        std::cout << "DEBUG: No path found!" << std::endl;
        currentPath.clear();
        currentPathPoints.clear();
        grid.clearPath();
    }
}

void Game::setAlgorithm(Pathfinder::Algorithm algo) {
    if (currentAlgorithm == algo) return;
    currentAlgorithm = algo;
    pathfinder = Pathfinder::create(currentAlgorithm);
    // Vorhandenen Pfad verwerfen, damit neu berechnet wird
    currentPath.clear();
    currentPathPoints.clear();
    grid.clearPath();
    std::cout << "Pfadfindungs-Algorithmus gewechselt zu ";
    switch (currentAlgorithm) {
        case Pathfinder::Algorithm::BFS:     std::cout << "BFS\n"; break;
        case Pathfinder::Algorithm::DIJKSTRA:std::cout << "Dijkstra\n"; break;
    }
}

Direction Game::getNextAIMove() {
    if (currentPath.empty()) return Direction::NONE;

    Direction planned = currentPath.front();
    Direction oldDir = snake.getDirection();

    snake.setDirection(planned);

    if (snake.getDirection() != planned && snake.getBody().size() < 3) {
        // 180°-Turn blockiert - finde Alternative
        Point head = snake.getHeadPosition();
        Direction alternatives[] = {
                (oldDir == Direction::LEFT || oldDir == Direction::RIGHT) ? Direction::UP : Direction::LEFT,
                (oldDir == Direction::LEFT || oldDir == Direction::RIGHT) ? Direction::DOWN : Direction::RIGHT
        };

        for (Direction dir : alternatives) {
            Point offset = (dir == Direction::UP) ? Point{0,-1} :
                           (dir == Direction::DOWN) ? Point{0,1} :
                           (dir == Direction::LEFT) ? Point{-1,0} : Point{1,0};

            if (grid.isInBounds(head + offset) && grid.getCell(head + offset) != CellType::WALL) {
                snake.setDirection(dir);
                break;
            }
        }

    } else {
        // Erfolgreich gesetzt
        currentPath.erase(currentPath.begin());

        currentPath.clear();
    }

    return snake.getDirection();
}

void Game::toggleAutoPlay() {
    autoPlay = !autoPlay;
    if (autoPlay) {
        std::cout << "AutoPlay aktiviert - Druecke SPACE zum Deaktivieren\n";
        currentPath.clear(); // Pfad zurücksetzen
    } else {
        std::cout << "AutoPlay deaktiviert - Verwende WASD/Pfeiltasten\n";
        currentPath.clear();
    }
}
void Game::reset() {
    // Reset das Spiel Status
    state = GameState::RUNNING;
    score = 0;
    hud.updateScore(score, renderer, windowWidth);

    // Reset das Grid
    grid.clear();

    //Rest Waende
    grid.initializeWalls();

    // Reset die Schlange pos
    snake = Snake(Point(grid.getBreite() / 2, grid.getHohe() / 2));

    // Clear Essen positions
    foodPositions.clear();

    // HINZUFÜGEN: Pfad-Variablen zurücksetzen
    currentAlgorithm = Pathfinder::Algorithm::BFS;
    pathfinder = Pathfinder::create(currentAlgorithm);

    currentPath.clear();
    currentPathPoints.clear();
    grid.clearPath();


    // Spawn neue Essen
    for (int i = 0; i < maxFoodItems; ++i) {
        spawnFood();
    }
    grid.clearObstacles();
    generateObstaclesForLevel();
}

Point Game::findClosestFood(const Point& position) const {
    // Wenn kein Food da ist, gib einen ungültigen Punkt zurück
    if (foodPositions.empty()) {
        return Point(-1, -1);
    }

    // Initialisiere die Suche: als "aktuell nächstes Food" das erste in der Liste
    Point closest = foodPositions[0];
    // Setze die minimale Distanz hoch, damit jeder echte Wert kleiner ist
    int minDistance = std::numeric_limits<int>::max();

    // Durchlaufe alle Essen-Positionen
    for (const Point& food : foodPositions) {
        // Berechne die Manhattan-Distanz
        int distance = std::abs(position.x - food.x) + std::abs(position.y - food.y);

        // Wenn diese Distanz kleiner ist als bisher die kleinste,
        // merke sie dir als neue minDistance und speichere das Essen
        if (distance < minDistance) {
            minDistance = distance;
            closest = food;
        }
    }

    // Gib die Position des nächsten Foods zurück
    return closest;
}

bool Game::handleEvents() {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) {
            return false;
        }

        if (e.type == SDL_KEYDOWN) {
            switch (e.key.keysym.sym) {
                case SDLK_w:
                case SDLK_UP:
                    if (!autoPlay) processInput(Direction::UP);  // Nur wenn AutoPlay aus ist
                    break;
                case SDLK_s:
                case SDLK_DOWN:
                    if (!autoPlay) processInput(Direction::DOWN);
                    break;
                case SDLK_a:
                case SDLK_LEFT:
                    if (!autoPlay) processInput(Direction::LEFT);
                    break;
                case SDLK_d:
                case SDLK_RIGHT:
                    if (!autoPlay) processInput(Direction::RIGHT);
                    break;
                case SDLK_1:
                    setAlgorithm(Pathfinder::Algorithm::BFS);
                    break;
                case SDLK_2:
                    setAlgorithm(Pathfinder::Algorithm::DIJKSTRA);
                    break;
                case SDLK_SPACE:
                    updatePathfinding();
                    toggleAutoPlay();
                    break;
                case SDLK_p:
                    if (state == GameState::RUNNING) {
                        state = GameState::PAUSED;
                    } else if (state == GameState::PAUSED) {
                        state = GameState::RUNNING;
                    }
                    break;
                case SDLK_r:
                    if (state == GameState::GAME_OVER) {
                        reset();
                    }
                    break;
                case SDLK_q:
                case SDLK_ESCAPE:
                    return false;
            }
        }
    }
    return true;
}

void Game::run() {
    if (!initSDL()) {
        return;
    }

    bool running = true;

    while (running) {
        running = handleEvents();
        update();
        render();

        SDL_Delay(16); // ~60 FPS
    }

    cleanup();
}