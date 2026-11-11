#include "Player.h"

#include "Room.h"
#include "raymath.h"

Player::Player(Vector2 startPosition) : position(startPosition)
{
}

void Player::update(Vector2 movement, float deltaTime, const Room& room)
{
    const Vector2 displacement = Vector2Scale(Vector2Normalize(movement), speed * deltaTime);
    position = room.move(position, displacement, Config::playerHalfSize);
}

void Player::aimAt(Vector2 mouseWorld)
{
    const Vector2 aim = Vector2Subtract(mouseWorld, position);
    if (Vector2LengthSqr(aim) > 0.0001f)
        direction = Vector2Normalize(aim);
}

void Player::draw() const
{
    const float size = Config::playerHalfSize;
    DrawRectangleRec({position.x - size, position.y - size, size * 2, size * 2}, Config::accent);
    const Vector2 aimEnd = Vector2Add(position, Vector2Scale(direction, Config::aimLength));
    DrawLineEx(position, aimEnd, 3.0f, Config::text);
    DrawCircleV(aimEnd, 4.0f, Config::text);
}
