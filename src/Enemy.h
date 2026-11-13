#pragma once

#include "Config.h"

class Room;
class Player;

enum class EnemyType { Chaser };

class Enemy
{
public:
    explicit Enemy(Vector2 startPosition) : position(startPosition) {}
    void update(float deltaTime, const Room& room, Player& player);
    void takeDamage(int damage);
    void takeMeleeDamage(int damage, unsigned int swing);
    void draw() const;
    bool isAlive() const { return health > 0; }
    Vector2 getPosition() const { return position; }
    Rectangle getBounds() const;
    int getHealth() const { return health; }
    EnemyType getType() const { return type; }

private:
    Vector2 position;
    int health = Config::enemyHealth;
    EnemyType type = EnemyType::Chaser;
    float attackCooldown = 0.0f;
    unsigned int lastHitSwing = 0;
};
