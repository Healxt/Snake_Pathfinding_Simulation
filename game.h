#pragma once

#include <random>
#include <chrono>
#include <vector>
#include "grid.h"
#include "Snake.h"
#include "Pathfinder.h"
#include "ObstacleGenerator.h"
#include "HUD.h"

enum class GameState{
    RUNNING,
    PAUSED,
    GAME_OVER,
};

class Game{
private:
    Grid grid;
    std::vector<Point> foodPositions;
    Snake snake;
    GameState state;
    int score;

    // Pathfinding
    bool autoPlay = false;                        // AutoPlay Status
    std::vector<Direction> currentPath;           // Pfad als Richtungen
    std::vector<Point> currentPathPoints;         // Pfad als Punkte für Rendering
    std::unique_ptr<Pathfinder> pathfinder;
    Pathfinder::Algorithm currentAlgorithm;


    // SDL Komponenten
    SDL_Window* window;
    SDL_Renderer* renderer;
    int windowWidth, windowHeight;
    int cellSize;
    HUD hud;

    // Spiel Einstellungen
    int maxFoodItems = 3; // Maximum number of food items on the grid at once
    Uint32 lastMoveTime;
    Uint32 moveDelay; // milliseconds between moves

    // Random nummer generateror
    std::mt19937 rng;

    //Level und Hindernisse
    int level = 1;
    ObstacleGenerator obstacleGenerator;

    // Hilfer Methoden fuer Essen spawen und entfernen
    void spawnFood();
    void removeFood(const Point& foodPos);

public:
    Game(int breite = 40, int hohe = 30, int windowW = 800, int windowH = 600);
    ~Game();

    // SDL Initialisierung und cleanup
    bool initSDL();
    void cleanup();

    // Spiel loop Methoden
    void processInput(Direction dir);
    void update();
    void render();

    // Spiel status Methoden
    bool isGameOver() const { return state == GameState::GAME_OVER; }
    bool isPaused() const { return state == GameState::PAUSED; }
    void togglePause() { state = (state == GameState::PAUSED) ? GameState::RUNNING : GameState::PAUSED; }
    void reset();

    // Essen Einstellungen
    void setMaxFoodItems(int count) { maxFoodItems = count; }
    int getMaxFoodItems() const { return maxFoodItems; }

    // Inhalte des Spiels oder Objekte des Spiels
    int getScore() const { return score; }
    GameState getState() const { return state; }
    void setState(GameState newState) { state = newState; }
    const Grid& getGrid() const { return grid; }
    const Snake& getSnake() const { return snake; }
    const std::vector<Point>& getFoodPositions() const { return foodPositions; }

    void generateObstaclesForLevel();

    // Pathfinding-Funktionen
    void setAlgorithm(Pathfinder::Algorithm algo);
    void updatePathfinding();
    Direction getNextAIMove();
    void toggleAutoPlay();

    // sucht den naechsten Essen in der Naehe von einer Position
    Point findClosestFood(const Point& position) const;

    // SDL-spezifische Methoden
    bool handleEvents();
    void run();
};