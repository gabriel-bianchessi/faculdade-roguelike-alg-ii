#pragma once

#include "Enemy.h"

#include <vector>

class Room;
class Player;

enum class WeaponCategory { Melee, Ranged };
enum class WeaponType { Sword, Pistol };

struct WeaponConfig
{
    WeaponType type;
    WeaponCategory category;
    int damage;
    float interval;
    float reach;
    float width;
    float projectileSpeed;
    float projectileLifetime;
};

inline constexpr WeaponConfig sword{WeaponType::Sword, WeaponCategory::Melee, 20, 0.4f, 76, 64, 0, 0};
inline constexpr WeaponConfig pistol{WeaponType::Pistol, WeaponCategory::Ranged, 8, 0.3f, 0, 0, 650, 2};

struct Projectile
{
    Vector2 position;
    Vector2 velocity;
    int damage;
    float remaining;
};

class Combat
{
public:
    void switchWeapon();
    void update(float deltaTime, bool attack, const Player& player, const Room& room);
    void draw(const Player& player) const;
    const WeaponConfig& getWeapon() const { return equipped == WeaponType::Sword ? sword : pistol; }
    std::vector<Enemy>& getEnemies() { return enemies; }
    const std::vector<Enemy>& getEnemies() const { return enemies; }
    const std::vector<Projectile>& getProjectiles() const { return projectiles; }

private:
    WeaponType equipped = WeaponType::Sword;
    float swordCooldown = 0;
    float pistolCooldown = 0;
    float swingRemaining = 0;
    unsigned int swingNumber = 0;
    Vector2 swingPosition = {};
    Vector2 swingDirection = {1, 0};
    std::vector<Enemy> enemies;
    std::vector<Projectile> projectiles;
    void attack(const Player& player);
    void resolveSwing(const Room& room);
};
