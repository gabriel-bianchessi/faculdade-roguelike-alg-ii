#include "Enemy.h"

#include "Player.h"
#include "Room.h"
#include "raymath.h"

#include <algorithm>

Rectangle Enemy::getBounds() const
{
    const float half = Config::enemyHalfSize;
    return {position.x - half, position.y - half, half * 2, half * 2};
}

void Enemy::takeDamage(int damage)
{
    health = std::max(0, health - damage);
}

void Enemy::takeMeleeDamage(int damage, unsigned int swing)
{
    if (lastHitSwing == swing) return;
    lastHitSwing = swing;
    takeDamage(damage);
}

void Enemy::update(float deltaTime, const Room& room, Player& player)
{
    if (!isAlive()) return;
    attackCooldown = std::max(0.0f, attackCooldown - deltaTime);
    const Vector2 difference = Vector2Subtract(player.getPosition(), position);
    const float reach = Config::enemyHalfSize + Config::playerHalfSize + 4;
    if (Vector2Length(difference) > reach)
        position = room.move(position, Vector2Scale(Vector2Normalize(difference),
                             Config::enemySpeed * deltaTime), Config::enemyHalfSize);
    if (Vector2Distance(position, player.getPosition()) <= reach && attackCooldown <= 0)
    {
        player.takeDamage(Config::enemyDamage);
        attackCooldown = Config::enemyAttackInterval;
    }
}

void Enemy::draw() const
{
    DrawRectangleRec(getBounds(), MAROON);
    DrawRectangle(static_cast<int>(position.x - 16), static_cast<int>(position.y - 24),
                  32, 4, DARKGRAY);
    DrawRectangle(static_cast<int>(position.x - 16), static_cast<int>(position.y - 24),
                  32 * health / Config::enemyHealth, 4, RED);
}
