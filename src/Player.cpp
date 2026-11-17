#include "Player.h"

#include "Room.h"
#include "raymath.h"

#include <algorithm>
#include <cmath>

Player::Player(Vector2 startPosition) : position(startPosition)
{
}

void Player::update(Vector2 movement, float deltaTime, const Room& room, bool dash)
{
    collectionFlash = std::max(0.0f, collectionFlash-deltaTime);
    bonusRemaining = std::max(0.0f, bonusRemaining - deltaTime);
    damageProtection = std::max(0.0f, damageProtection - deltaTime);
    dashCooldown = std::max(0.0f, dashCooldown - deltaTime);
    if (dash && dashCooldown <= 0.0f)
    {
        dashRemaining = Config::dashDuration;
        dashCooldown = Config::dashCooldown;
        dashDirection = Vector2LengthSqr(movement) > 0 ? Vector2Normalize(movement) : direction;
    }
    const Vector2 start = position;
    const float dashTime = std::min(deltaTime, dashRemaining);
    position = room.move(position, Vector2Scale(dashDirection, Config::dashSpeed * dashTime),
                         Config::playerHalfSize);
    distance += Vector2Distance(start, position);
    const Vector2 afterDash = position;
    dashRemaining = std::max(0.0f, dashRemaining - deltaTime);
    const Vector2 displacement = Vector2Scale(Vector2Normalize(movement), speed * (deltaTime - dashTime));
    position = room.move(position, displacement, Config::playerHalfSize);
    distance += Vector2Distance(afterDash, position);
}

int Player::heal(int amount)
{
    const int restored = std::clamp(amount, 0, maxHealth - health);
    health += restored;
    healing += restored;
    return restored;
}

void Player::boostDamage(float percent, float duration)
{
    bonusPercent = percent;
    bonusRemaining = duration;
}

bool Player::isInvulnerable() const
{
    return damageProtection > 0 ||
           dashRemaining > Config::dashDuration - Config::dashInvulnerability;
}

bool Player::takeDamage(int damage)
{
    if (health <= 0 || damage <= 0 || isInvulnerable()) return false;
    const int absorbed = std::min(shield, damage);
    shield -= absorbed;
    health = std::max(0, health - (damage - absorbed));
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
    if (shield > 0) DrawCircleLines(position.x, position.y, Config::playerHalfSize+7, SKYBLUE);
    if (collectionFlash > 0) DrawCircleLines(position.x, position.y, 22+(0.25f-collectionFlash)*80, GOLD);
    const float size = Config::playerHalfSize;
    DrawRectangleRec({position.x - size, position.y - size, size * 2, size * 2}, isInvulnerable() ? SKYBLUE : Config::accent);
    const Vector2 aimEnd = Vector2Add(position, Vector2Scale(direction, Config::aimLength));
    DrawCircleV(aimEnd, 2.0f, Fade(Config::text,0.5f));
}

void Player::gainExperience(int amount)
{
    experience += amount;
    while (experience >= getNextLevelCost())
    {
        experience -= getNextLevelCost();
        ++level;
        ++attributePoints;
    }
}

void Player::recordDefeat(int reward)
{
    ++defeatedCount;
    gainExperience(reward);
}

bool Player::spendPoint(Attribute attribute)
{
    if (attributePoints <= 0) return false;
    --attributePoints;
    switch (attribute)
    {
        case Attribute::Strength: ++attributes.strength; break;
        case Attribute::Vitality:
            ++attributes.vitality;
            maxHealth = Config::initialHealth + attributes.vitality * Config::vitalityBonus;
            heal(Config::vitalityBonus);
            break;
        case Attribute::Agility: ++attributes.agility; break;
    }
    speed = Config::initialSpeed * (1 + attributes.agility * Config::agilityBonus);
    return true;
}

int Player::getScore(int discoveredRooms) const
{
    return std::max(0, Config::scoreEnemy * defeatedCount + Config::scoreItem * itemsCollected + Config::scoreRoom * discoveredRooms +
        (victorious ? Config::scoreVictory : 0) - static_cast<int>(std::floor(distance / Config::scoreDistanceUnit)) - Config::scoreHealingPenalty * healing);
}
