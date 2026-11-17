#pragma once

#include "Config.h"
#include <vector>

class Room;
class Player;

enum class BossState { Hunt, ChargeWarning, Charge, FanWarning, Recovery };

enum class EnemyType { Chaser, Shooter, Patrol, Boss };

class Enemy
{
public:
    explicit Enemy(Vector2 startPosition, EnemyType kind = EnemyType::Chaser) : position(startPosition), type(kind), wanderTarget(startPosition)
    { if (kind == EnemyType::Boss) health = Config::bossHealth; }
    void update(float deltaTime, Room& room, Player& player);
    void takeDamage(float damage);
    void takeMeleeDamage(float damage, unsigned int swing);
    void draw() const;
    bool isAlive() const { return health > 0; }
    Vector2 getPosition() const { return position; }
    Rectangle getBounds() const;
    float getHealth() const { return health; }
    int getExperienceReward() const { return type == EnemyType::Boss ? Config::bossExperience : Config::commonExperience; }
    BossState getBossState() const { return bossState; }
    EnemyType getType() const { return type; }

private:
    BossState bossState = BossState::Hunt;
    float stateTimer = Config::bossHuntDuration;
    int attackCycle = 0;
    Vector2 chargeDirection = {1,0};
    void updateBoss(float deltaTime, Room& room, Player& player);
    float hitFlash = 0;
    float pathCooldown = 0;
    float wanderTimer = 0;
    unsigned int wanderStep = 0;
    std::vector<Vector2> path;
    std::size_t waypoint = 0;
    Vector2 position;
    float health = Config::enemyHealth;
    EnemyType type = EnemyType::Chaser;
    Vector2 wanderTarget;
    float attackCooldown = 0.0f;
    unsigned int lastHitSwing = 0;
};
