#pragma once

#include "Config.h"
#include <array>

class Room;

struct Attributes
{
    int strength = 0;
    int vitality = 0;
    int agility = 0;
};

enum class Attribute { Strength, Vitality, Agility };
enum class PlayerAnimation { Idle, WalkDown, WalkLeft, WalkRight, WalkUp, Attack, Hurt, Death };

class Player
{
public:
    explicit Player(Vector2 startPosition = {});
    void update(Vector2 movement, float deltaTime, const Room& room, bool dash = false);
    void aimAt(Vector2 mouseWorld);
    void enterRoom(Vector2 arrival);
    void draw(Texture2D knightTexture = {}) const;
    void playSwordAnimation();
    void updateDeathAnimation(float deltaTime);
    PlayerAnimation getAnimation() const { return animation; }
    int getAnimationFrame() const;
    bool usesSwordAttackSprite() const { return animation == PlayerAnimation::Attack; }
    bool takeDamage(int damage);
    int heal(int amount);
    void grantShield(int amount) { shield = amount; }
    void boostDamage(float percent, float duration);
    int getShield() const { return shield; }
    float getBonusRemaining() const { return bonusRemaining; }
    float getDamageMultiplier() const { return (1 + attributes.strength * Config::strengthBonus) *
        (bonusRemaining > 0 ? 1 + bonusPercent : 1); }
    void recordCollection(int kind = -1)
    {
        ++itemsCollected;
        collectionFlash = 0.25f;
        if (kind >= 0 && kind < static_cast<int>(collectedKinds.size())) collectedKinds[kind] = true;
    }
    bool hasCollected(int kind) const { return collectedKinds.at(kind); }
    int getItemsCollected() const { return itemsCollected; }
    void gainExperience(int amount);
    void recordDefeat(int experience);
    bool spendPoint(Attribute attribute);
    int getLevel() const { return level; }
    int getExperience() const { return experience; }
    int getNextLevelCost() const { return Config::levelCost * level; }
    int getAttributePoints() const { return attributePoints; }
    const Attributes& getAttributes() const { return attributes; }
    int getDefeatedCount() const { return defeatedCount; }
    double getDistance() const { return distance; }
    int getHealing() const { return healing; }
    bool hasWon() const { return victorious; }
    void recordVictory() { victorious = true; }
    int getScore(int discoveredRooms, float activeSeconds = 0) const;
    bool isInvulnerable() const;
    float getDashCooldown() const { return dashCooldown; }


    Vector2 getPosition() const { return position; }
    Vector2 getDirection() const { return direction; }
    int getHealth() const { return health; }
    int getMaxHealth() const { return maxHealth; }
    float getSpeed() const { return speed; }

private:
    PlayerAnimation animation = PlayerAnimation::Idle;
    PlayerAnimation walkDirection = PlayerAnimation::WalkDown;
    float animationTime = 0;
    float attackAnimationRemaining = 0;
    float hurtAnimationRemaining = 0;
    bool moving = false;
    void updateAnimation(Vector2 displacement, float deltaTime);
    Attributes attributes;
    int level = 1;
    int experience = 0;
    int attributePoints = 0;
    int defeatedCount = 0;
    int healing = 0;
    double distance = 0;
    bool victorious = false;
    float collectionFlash = 0;
    int shield = 0;
    int itemsCollected = 0;
    std::array<bool,9> collectedKinds{};
    float bonusPercent = 0;
    float bonusRemaining = 0;
    float damageProtection = 0.0f;
    float dashRemaining = 0.0f;
    float dashCooldown = 0.0f;
    Vector2 dashDirection = {};
    Vector2 position;
    Vector2 direction = {1.0f, 0.0f};
    int health = Config::initialHealth;
    int maxHealth = Config::initialHealth;
    float speed = Config::initialSpeed;
};
