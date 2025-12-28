#include <iostream>
#include <vector>
#include <string>
#include <stack>
#include <random>

#ifdef _WIN32
#include <windows.h>
#endif

#ifdef _WIN32
void clearScreen() { system("cls"); }
#else 
void clearScreen() { system("clear"); }
#endif

// Directions the player can face 
enum Direction { NORTH, EAST, SOUTH, WEST };

// Player state
struct Player {
	size_t x = 1;
	size_t y = 1;
	Direction dir = NORTH;
};

// Simple maze generator
std::vector<std::string> generateMaze(int w, int h) {
    // Ensure odd dimensions (DFS maze works best with odd sizes)
    if (w % 2 == 0) w++;
    if (h % 2 == 0) h++;

    // Fill maze with walls
    std::vector<std::string> maze(h, std::string(w, '#'));

    std::stack<std::pair<int,int>> stack;
    std::random_device rd;
    std::mt19937 rng(rd());

    // Start position
    stack.push({1, 1});
    maze[1][1] = ' ';

    // Lambda to get valid neighbors two steps away
    auto neighbors = [&](int x, int y) {
        std::vector<std::pair<int,int>> dirs;
        if (x > 2)         dirs.push_back({x - 2, y});
        if (x < w - 3)     dirs.push_back({x + 2, y});
        if (y > 2)         dirs.push_back({x, y - 2});
        if (y < h - 3)     dirs.push_back({x, y + 2});
        return dirs;
    };

    // DFS carving loop
    while (!stack.empty()) {
        auto [x, y] = stack.top();
        auto dirs = neighbors(x, y);

        // Collect unvisited neighbors
        std::vector<std::pair<int,int>> unvisited;
        for (auto& d : dirs) {
            if (maze[d.second][d.first] == '#')
                unvisited.push_back(d);
        }

        if (unvisited.empty()) {
            stack.pop();
            continue;
        }

        // Pick a random unvisited neighbor
        std::uniform_int_distribution<int> dist(0, unvisited.size() - 1);
        auto [nx, ny] = unvisited[dist(rng)];

        // Carve passage between current cell and neighbor
        maze[(y + ny) / 2][(x + nx) / 2] = ' ';
        maze[ny][nx] = ' ';

        stack.push({nx, ny});
    }

    return maze;
}

std::vector<std::string> maze;   // global maze variable

// --- Helper functions ---

std::string directionName(Direction d) {
	switch (d) {
		case NORTH: return "north";
		case SOUTH: return "south";
		case EAST: return "east";
		case WEST: return "west";
	}
	return "";
}

std::string compassString(Direction d) {
	switch (d) {
		case NORTH: return "[Compass] Facing NORTH ↑";
		case SOUTH: return "[Compass] Facing SOUTH ↓";
		case EAST: return "[Compass] Facing EAST  →";
		case WEST: return "[Compass] Facing WEST  ←";
	}
	return "[Compass] Unknown";
}

// --- Movement ---

void turnLeft(Player& p, std::string& message) {
	p.dir = static_cast<Direction>((p.dir + 3) % 4);
	message = "You turn left. Now facing " + directionName(p.dir) + ".";
}
void turnRight(Player& p, std::string& message) {
	p.dir = static_cast<Direction>((p.dir + 1) % 4); 
	message = "You turn right. Now facing " + directionName(p.dir) + ".";
}
void moveForward(Player& p, std::string& message) {
    int nx = p.x;
    int ny = p.y;

    if (p.dir == NORTH) ny--;
    if (p.dir == SOUTH) ny++;
    if (p.dir == EAST)  nx++;
    if (p.dir == WEST)  nx--;

    // Bounds check
    if (nx < 0 || ny < 0 || ny >= (int)maze.size() || nx >= (int)maze[0].size()) {
        message = "You bump into the boundary. Still facing " + directionName(p.dir) + ".";
        return;
    }

    if (maze[ny][nx] == ' ') {
        p.x = nx;
        p.y = ny;
        message = "You move forward. Facing " + directionName(p.dir) + ".";
    } else {
        message = "You bump into a wall. Still facing " + directionName(p.dir) + ".";
    }
}
void moveBackward(Player& p, std::string& message) {
    int nx = p.x;
    int ny = p.y;

    if (p.dir == NORTH) ny++;
    if (p.dir == SOUTH) ny--;
    if (p.dir == EAST)  nx--;
    if (p.dir == WEST)  nx++;

    // Bounds check
    if (nx < 0 || ny < 0 || ny >= (int)maze.size() || nx >= (int)maze[0].size()) {
        message = "You back into the boundary. Still facing " + directionName(p.dir) + ".";
        return;
    }

    if (maze[ny][nx] == ' ') {
        p.x = nx;
        p.y = ny;
        message = "You step backward. Facing " + directionName(p.dir) + ".";
    } else {
        message = "You step back into a wall. Still facing " + directionName(p.dir) + ".";
    }
}

int main() {

    while (true) {   // NEW OUTER LEVEL LOOP

        // Reset player for this level
        Player player;
        std::string message = "Welcome to Grid Maze!";

        // Generate a fresh maze for this level
        maze = generateMaze(21, 21);  // use the global
        maze[maze.size() - 2][maze[0].size() - 1] = ' ';  // exit hole

        // INNER GAME LOOP
        while (true) {
            clearScreen();

            // Display compass + message
            std::cout << compassString(player.dir) << "\n";
            std::cout << message << "\n\n";

            // Display maze with player
            for (size_t y = 0; y < maze.size(); y++) {
                for (size_t x = 0; x < maze[y].size(); x++) {

                    if (x == player.x && y == player.y) {
                        char icon = '^';  // default

                        switch (player.dir) {
                            case NORTH: icon = '^'; break;
                            case EAST:  icon = '>'; break;
                            case SOUTH: icon = 'v'; break;
                            case WEST:  icon = '<'; break;
                        }

                        std::cout << icon;   // player pointer
                    }
                    else {
                        std::cout << maze[y][x];
                    }
                }
                std::cout << "\n";
            }

            std::cout << "\nControls: W = forward, S = backward, A = left, D = right, Q = quit\n";
            std::cout << "by octogone\n";
            char input;
            std::cin >> input;

            if (input == 'q' || input == 'Q')
                return 0;  // quit entire game

            if (input == 'a' || input == 'A') turnLeft(player, message);
            else if (input == 'd' || input == 'D') turnRight(player, message);
            else if (input == 'w' || input == 'W') moveForward(player, message);
            else if (input == 's' || input == 'S') moveBackward(player, message);
            else message = "Unknown command.";

            // WIN CONDITION
            if (player.x == maze[0].size() - 1 && player.y == maze.size() - 2) {
                clearScreen();
                std::cout << "You slip through the opening and escape the maze!\n";
                break;  // break inner loop only
            }
        }

        // AFTER BEATING THE LEVEL
        std::cout << "\nPlay another level? (Y/N)\n";
        char again;
        std::cin >> again;

        if (again != 'y' && again != 'Y')
            break;  // break outer loop → exit game
    }

    return 0;
}

