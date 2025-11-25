#include "Pathfinder.h"

std::vector<Point> Pathfinder::getNeighbors(const Point &p, const Grid &grid, const Snake &snake) const {
    std::vector<Point> neighbors;
    neighbors.reserve(Directions::ALL_DIRECTIONS.size());

    for (const Point &dir : Directions::ALL_DIRECTIONS) {
        Point candidate = p + dir;  // absolute neighbor

        // PRUFE, ob im Grid und kein Wand
        if(grid.isInBounds(candidate) && grid.getCell(candidate) != CellType::WALL){
            //PRUFE, ob es keinen Teil vom Schlangen Körper (außer den Schwanz von der Schlange)
            bool isSnakeBody = false;
            const std::deque<Point> &snakeBody = snake.getBody();

            //SKIPPE die Prüfung vom letzte Teil der Schlange(Schwanz),weil es sich bewegt
            size_t checkUntil = snakeBody.size();
            if (checkUntil > 1){
                // Nur SKIPPE der Schwanz Prüfung, wenn die Schlange sich nicht vergrößert
                checkUntil -= 1;
            }
            for (size_t i = 0; i < checkUntil; i++) {
                if (snakeBody[i] == candidate){
                    isSnakeBody = true;
                    break;
                }
            }
            if (!isSnakeBody){
                neighbors.push_back(candidate);
            }
        }
    }
    return neighbors;
}

std::vector<Direction> Pathfinder::pathToDirections(const std::vector<Point> &path, Point startpos)const{
    std::vector<Direction> directions;

    if (path.empty()) {
        return directions;
    }

    Point current = startpos;

    for (const auto &next : path) {
        directions.push_back(getDirection(current,next));
        current = next;
    }

    return directions;
}

Direction Pathfinder::getDirection(const Point &from, const Point &to) const {
    if (from.x < to.x) return Direction::RIGHT;
    if (from.x > to.x) return Direction::LEFT;
    if (from.y > to.y) return Direction::UP;
    if (from.y < to.y) return Direction::DOWN;

    return Direction::NONE;
}

// BFS Pathfinder Implementation
std::vector<Point> BFSPathfinder::findPath(Point start, Point goal, const Grid& grid, const Snake& snake) {
    std::queue<Point> queue;
    std::unordered_set<Point, PointHash> visited;
    std::unordered_map<Point, Point, PointHash> cameFrom;

    // Start von den kopf der snake
    queue.push(start);
    visited.insert(start);

    bool foundPath = false;

    while (!queue.empty() && !foundPath) {
        Point current = queue.front();
        queue.pop();

        // Check if den goal erreicht haben
        if (current == goal) {
            foundPath = true;
            break;
        }

        // Get (up, down, left, right)
        std::vector<Point> neighbors = getNeighbors(current, grid, snake);

        for (const auto& neighbor : neighbors) {
            if (visited.find(neighbor) == visited.end()) {
                queue.push(neighbor);
                visited.insert(neighbor);
                cameFrom[neighbor] = current;
            }
        }
    }

    // Wenn wir einen Pfad gefunden haben, rekonstruieren wir ihn
    std::vector<Point> path;
    if (foundPath) {
        Point current = goal;
        while (current != start) {
            path.push_back(current);
            current = cameFrom[current];
        }

        // Kehren Sie den Pfad um, um vom Start zum Ziel zu gelangen
        std::reverse(path.begin(), path.end());
    }

    return path;
}

// Dijkstra Pathfinder Implementierung
std::vector<Point> DijkstraPathfinder::findPath(Point start, Point goal, const Grid& grid, const Snake& snake) {
    // Priority queue fuer Dijkstra's algorithm
    using PQElement = std::pair<int, Point>;
    std::priority_queue<PQElement, std::vector<PQElement>, std::greater<PQElement>> pq;

    // Datenstrukturen verfolgen
    std::unordered_map<Point, int, PointHash> distance;
    std::unordered_map<Point, Point, PointHash> cameFrom;
    std::unordered_set<Point, PointHash> visited;

    // Initialisieren Sie Entfernungen bis unendlich (Max von int !!)
    for (int y = 0; y < grid.getHohe(); y++) {
        for (int x = 0; x < grid.getBreite(); x++) {
            Point p(x, y);
            distance[p] = std::numeric_limits<int>::max();
        }
    }

    // Start Knote hat 0 Entfernung
    distance[start] = 0;
    pq.push(std::make_pair(0, start));

    bool foundPath = false;

    while (!pq.empty() && !foundPath) {
        // Knoten mit minimaler Distanz abrufen
        Point current = pq.top().second;
        pq.pop();

        // Überspringen, wenn bereits besucht
        if (visited.find(current) != visited.end()) {
            continue;
        }

        // Als besucht markieren
        visited.insert(current);

        // Prüfen, ob wir das Ziel erreicht haben
        if (current == goal) {
            foundPath = true;
            break;
        }

        // Nachbarn verarbeiten
        std::vector<Point> neighbors = getNeighbors(current, grid, snake);

        for (const auto& neighbor : neighbors) {
            // Die Kosten für den Umzug zum Nachbarn betragen in dieser Implementierung immer 1
            int newDist = distance[current] + 1;

            if (newDist < distance[neighbor]) {
                distance[neighbor] = newDist;
                cameFrom[neighbor] = current;
                pq.push(std::make_pair(newDist, neighbor));
            }
        }
    }

    // Wenn wir einen Pfad gefunden haben, rekonstruieren wir ihn
    std::vector<Point> path;
    if (foundPath) {
        Point current = goal;
        while (current != start) {
            path.push_back(current);
            current = cameFrom[current];
        }

        // Kehren Sie den Pfad um, um vom Start zum Ziel zu gelangen
        std::reverse(path.begin(), path.end());
    }

    return path;
}

std::unique_ptr<Pathfinder> Pathfinder::create(Pathfinder::Algorithm algo) {
    switch (algo) {
        case Algorithm::BFS:
            return std::make_unique<BFSPathfinder>();

        case Algorithm::DIJKSTRA:
            return std::make_unique<DijkstraPathfinder>();

        default:
            // Fallback, falls nötig
            return std::make_unique<BFSPathfinder>();
    }
}