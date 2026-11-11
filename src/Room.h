#pragma once

#include "Config.h"

#include <vector>

class Room
{
public:
    explicit Room(float width = Config::roomWidth, float height = Config::roomHeight);
    Rectangle getBounds() const { return bounds; }
    Vector2 getSpawn() const { return spawn; }
    const std::vector<Rectangle>& getSolids() const { return solids; }
    Vector2 move(Vector2 position, Vector2 displacement, float halfSize) const;
    Camera2D getCamera(int screenWidth, int screenHeight, Vector2 focus) const;
    void draw() const;

private:
    Rectangle bounds;
    Vector2 spawn;
    std::vector<Rectangle> solids;
};
