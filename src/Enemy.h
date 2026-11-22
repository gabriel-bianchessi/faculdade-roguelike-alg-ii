#pragma once

#include "Config.h"
#include <vector>

class Room;
class Player;

enum class BossState { Hunt, ChargeWarning, Charge, FanWarning, Recovery };

enum class EnemyType { Chaser, Shooter, Patrol, Boss };
enum class EnemyAnimation { Idle, WalkDown, WalkLeft, WalkRight, WalkUp, Attack, Hurt, Death,
                            ChargeWarning, Charge, FanWarning, FanAttack, Recovery };

class Enemy
{
public:
    explicit Enemy(Vector2 startPosition, EnemyType kind = EnemyType::Chaser) : position(startPosition), type(kind), wanderTarget(startPosition)
    { if (kind == EnemyType::Boss) health = Config::bossHealth; }
    void update(float deltaTime, Room& room, Player& player);
    void takeDamage(float damage);
    void takeMeleeDamage(float damage, unsigned int swing);
    void draw(Texture2D texture = {}) const;
    EnemyAnimation getAnimation() const { return animation; }
    int getAnimationFrame() const;
    int getSpriteRow() const;
    void updateDeathAnimation(float deltaTime);
    bool deathAnimationFinished() const { return !isAlive() && animationTime >= 0.64f; }
    bool isAlive() const { return health > 0; }
    Vector2 getPosition() const { return position; }
    Rectangle getBounds() const;
    float getHealth() const { return health; }
    int getExperienceReward() const { return type == EnemyType::Boss ? Config::bossExperience : Config::commonExperience; }
    BossState getBossState() const { return bossState; }
    EnemyType getType() const { return type; }

private:
    EnemyAnimation animation = EnemyAnimation::Idle;
    EnemyAnimation walkDirection = EnemyAnimation::WalkDown;
    float animationTime = 0;
    float attackAnimationRemaining = 0;
    float hurtAnimationRemaining = 0;
    bool moving = false;
    Vector2 attackDirection = {1,0};
    void updateAnimation(Vector2 displacement, float deltaTime);
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
