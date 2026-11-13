#pragma once

#include "Config.h"
#include "Enemy.h"

#include <vector>

struct Connection
{
    int from;
    int to;
    Rectangle door;
    Vector2 arrival;
    bool locked = false;
};

struct Cell
{
    bool walkable = false;
    bool explored = false;
    bool visible = false;
};

class Room
{
public:
    explicit Room(float width = Config::roomWidth, float height = Config::roomHeight, int identifier = 1);
    Rectangle getBounds() const { return bounds; }
    Vector2 getSpawn() const { return spawn; }
    const std::vector<Rectangle>& getSolids() const { return solids; }
    Vector2 move(Vector2 position, Vector2 displacement, float halfSize) const;
    Camera2D getCamera(int screenWidth, int screenHeight, Vector2 focus) const;
    void draw() const;
    void drawExploration() const;
    void reveal(Vector2 position);
    bool discover();
    int getId() const { return id; }
    bool isDiscovered() const { return discovered; }
    int getColumns() const { return columns; }
    int getRows() const { return rows; }
    const Cell& getCell(int column, int row) const { return cells.at(row * columns + column); }
    bool isWalkable(int column, int row) const;
    bool isVisible(Vector2 position) const;
    std::vector<Enemy>& getEnemies() { return enemies; }
    const std::vector<Enemy>& getEnemies() const { return enemies; }
    std::vector<Connection>& getConnections() { return connections; }
    const std::vector<Connection>& getConnections() const { return connections; }
    void placeKey(Vector2 position) { keyPosition = position; keyPresent = true; }
    bool collectKey(Vector2 position);
    bool hasKey() const { return keyPresent; }
    Vector2 getKeyPosition() const { return keyPosition; }


private:
    int id;
    bool discovered = false;
    int columns;
    int rows;
    std::vector<Cell> cells;
    std::vector<Enemy> enemies;
    std::vector<Connection> connections;
    bool keyPresent = false;
    Vector2 keyPosition = {};
    Rectangle bounds;
    Vector2 spawn;
    std::vector<Rectangle> solids;
};
