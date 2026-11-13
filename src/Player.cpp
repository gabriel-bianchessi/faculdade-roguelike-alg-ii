#include "Player.h"

#include "Room.h"
#include "raymath.h"

#include <algorithm>

Player::Player(Vector2 startPosition) : position(startPosition)
{
}

void Player::update(Vector2 movement, float deltaTime, const Room& room, bool dash)
{
    damageProtection = std::max(0.0f, damageProtection - deltaTime);
    dashCooldown = std::max(0.0f, dashCooldown - deltaTime);
    if (dash && dashCooldown <= 0.0f)
    {
        dashRemaining = Config::dashDuration;
        dashCooldown = Config::dashCooldown;
        dashDirection = Vector2LengthSqr(movement) > 0 ? Vector2Normalize(movement) : direction;
    }
    const float dashTime = std::min(deltaTime, dashRemaining);
    position = room.move(position, Vector2Scale(dashDirection, Config::dashSpeed * dashTime),
                         Config::playerHalfSize);
    dashRemaining = std::max(0.0f, dashRemaining - deltaTime);
    const Vector2 displacement = Vector2Scale(Vector2Normalize(movement), speed * (deltaTime - dashTime));
    position = room.move(position, displacement, Config::playerHalfSize);
}

bool Player::isInvulnerable() const
{
    return damageProtection > 0 ||
           dashRemaining > Config::dashDuration - Config::dashInvulnerability;
}

bool Player::takeDamage(int damage)
{
    if (health <= 0 || damage <= 0 || isInvulnerable()) return false;
    health = std::max(0, health - damage);
    damageProtection = Config::damageInvulnerability;
    return true;
}

void Player::enterRoom(Vector2 arrival)
{
    position = arrival;
    dashRemaining = 0;
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
    DrawRectangleRec({position.x - size, position.y - size, size * 2, size * 2}, isInvulnerable() ? SKYBLUE : Config::accent);
    const Vector2 aimEnd = Vector2Add(position, Vector2Scale(direction, Config::aimLength));
    DrawLineEx(position, aimEnd, 3.0f, Config::text);
    DrawCircleV(aimEnd, 4.0f, Config::text);
}
