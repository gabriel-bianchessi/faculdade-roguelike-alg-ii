#pragma once

#include "Config.h"

class Room;

class Player
{
public:
    explicit Player(Vector2 startPosition = {});
    void update(Vector2 movement, float deltaTime, const Room& room, bool dash = false);
    void aimAt(Vector2 mouseWorld);
    void enterRoom(Vector2 arrival);
    void draw() const;
    bool takeDamage(int damage);
    bool isInvulnerable() const;
    float getDashCooldown() const { return dashCooldown; }


    Vector2 getPosition() const { return position; }
    Vector2 getDirection() const { return direction; }
    int getHealth() const { return health; }
    int getMaxHealth() const { return maxHealth; }
    float getSpeed() const { return speed; }

private:
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
