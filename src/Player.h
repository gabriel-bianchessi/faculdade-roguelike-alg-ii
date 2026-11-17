#pragma once

#include "Config.h"

class Room;

struct Attributes
{
    int strength = 0;
    int vitality = 0;
    int agility = 0;
};

enum class Attribute { Strength, Vitality, Agility };

class Player
{
public:
    explicit Player(Vector2 startPosition = {});
    void update(Vector2 movement, float deltaTime, const Room& room, bool dash = false);
    void aimAt(Vector2 mouseWorld);
    void enterRoom(Vector2 arrival);
    void draw() const;
    bool takeDamage(int damage);
    int heal(int amount);
    void grantShield(int amount) { shield = amount; }
    void boostDamage(float percent, float duration);
    int getShield() const { return shield; }
    float getBonusRemaining() const { return bonusRemaining; }
    float getDamageMultiplier() const { return (1 + attributes.strength * Config::strengthBonus) *
        (bonusRemaining > 0 ? 1 + bonusPercent : 1); }
    void recordCollection() { ++itemsCollected; collectionFlash = 0.25f; }
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
    int getScore(int discoveredRooms) const;
    bool isInvulnerable() const;
    float getDashCooldown() const { return dashCooldown; }


    Vector2 getPosition() const { return position; }
    Vector2 getDirection() const { return direction; }
    int getHealth() const { return health; }
    int getMaxHealth() const { return maxHealth; }
    float getSpeed() const { return speed; }

private:
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
