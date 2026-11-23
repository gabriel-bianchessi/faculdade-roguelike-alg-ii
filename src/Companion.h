#pragma once

#include "raylib.h"
#include <vector>

class Player;
class Room;

class Companion
{
public:
    bool isActive() const { return active; }
    Vector2 getPosition() const { return position; }
    void recruit(Vector2 start);
    void enterRoom(Vector2 start);
    void update(float deltaTime, const Player& player, Room& room);
    void draw(Texture2D texture = {}) const;

private:
    bool active = false;
    Vector2 position{};
    float attackCooldown = 0;
    float animationTime = 0;
    float pathCooldown = 0;
    float strikeRemaining = 0;
    Vector2 strikeTarget{};
    bool moving = false;
    std::vector<Vector2> path;
    std::size_t waypoint = 0;
};
