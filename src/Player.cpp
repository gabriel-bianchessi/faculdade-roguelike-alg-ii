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
    attackAnimationRemaining = std::max(0.0f, attackAnimationRemaining-deltaTime);
    hurtAnimationRemaining = std::max(0.0f, hurtAnimationRemaining-deltaTime);
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
    updateAnimation(Vector2Subtract(position,start),deltaTime);
}

void Player::updateAnimation(Vector2 displacement, float deltaTime)
{
    moving = Vector2LengthSqr(displacement) > 0.000001f;
    if (moving)
        walkDirection = std::abs(displacement.x) > std::abs(displacement.y)
            ? (displacement.x < 0 ? PlayerAnimation::WalkLeft : PlayerAnimation::WalkRight)
            : (displacement.y < 0 ? PlayerAnimation::WalkUp : PlayerAnimation::WalkDown);
    const PlayerAnimation next = health <= 0 ? PlayerAnimation::Death
        : hurtAnimationRemaining > 0 ? PlayerAnimation::Hurt
        : attackAnimationRemaining > 0 ? PlayerAnimation::Attack
        : moving || walkDirection != PlayerAnimation::WalkDown ? walkDirection : PlayerAnimation::Idle;
    animationTime = next == animation ? animationTime+deltaTime : 0;
    animation = next;
}

void Player::playSwordAnimation()
{
    attackAnimationRemaining = 0.28f;
    if (health > 0 && hurtAnimationRemaining <= 0)
    {
        animation = PlayerAnimation::Attack;
        animationTime = 0;
    }
}

void Player::updateDeathAnimation(float deltaTime)
{
    if (health <= 0) animationTime += deltaTime;
}

int Player::getAnimationFrame() const
{
    switch (animation)
    {
        case PlayerAnimation::Idle: return static_cast<int>(animationTime/0.18f)%4;
        case PlayerAnimation::Attack: return std::min(3,static_cast<int>(animationTime/0.07f));
        case PlayerAnimation::Hurt: return std::min(1,static_cast<int>(animationTime/0.09f));
        case PlayerAnimation::Death: return std::min(3,static_cast<int>(animationTime/0.12f));
        default: return moving ? static_cast<int>(animationTime/0.1f)%4 : 0;
    }
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
    hurtAnimationRemaining = 0.24f;
    animation = health <= 0 ? PlayerAnimation::Death : PlayerAnimation::Hurt;
    animationTime = 0;
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

void Player::draw(Texture2D knightTexture) const
{
    if (shield > 0) DrawCircleLines(position.x, position.y, Config::playerHalfSize+7, SKYBLUE);
    if (collectionFlash > 0) DrawCircleLines(position.x, position.y, 22+(0.25f-collectionFlash)*80, GOLD);
    const float size = Config::playerHalfSize;
    if (IsTextureValid(knightTexture))
    {
        constexpr float scale = 0.4f;
        Rectangle source{getAnimationFrame()*192.0f,static_cast<int>(animation)*128.0f,192,128};
        const bool mirror = (animation == PlayerAnimation::Attack || animation == PlayerAnimation::Hurt ||
                             animation == PlayerAnimation::Death) && direction.x < 0;
        // A ancora dos quadros espelhados continua no centro do corpo, nao da celula.
        const float anchor = mirror ? 192-80 : 80;
        if (mirror) source.width = -source.width;
        DrawEllipse(position.x,position.y+size,15,5,Fade(BLACK,0.35f));
        DrawTexturePro(knightTexture,source,
            {position.x-anchor*scale,position.y+size-116*scale,192*scale,128*scale},
            {},0,isInvulnerable() ? Color{185,220,255,255} : WHITE);
    }
    else DrawRectangleRec({position.x - size, position.y - size, size * 2, size * 2}, isInvulnerable() ? SKYBLUE : Config::accent);
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
