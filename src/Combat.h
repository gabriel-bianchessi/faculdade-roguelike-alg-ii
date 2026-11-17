#pragma once

#include "Enemy.h"
#include "Weapon.h"

#include <vector>
#include <array>

class Room;
class Player;

struct Projectile
{
    Vector2 position;
    Vector2 velocity;
    float damage;
    float remaining;
};

class Combat
{
public:
    void switchWeapon();
    WeaponType replaceWeapon(WeaponType type);
    WeaponType getMeleeWeapon() const { return meleeWeapon; }
    WeaponType getRangedWeapon() const { return rangedWeapon; }
    void update(float deltaTime, bool attack, Player& player, Room& room);
    void draw(const Player& player, const Room& room, bool drawHeldWeapon = true,
              const std::array<Texture2D,3>& enemyTextures = {}) const;
    void clearTransient();
    const WeaponConfig& getWeapon() const { return weaponConfig(equipped); }
    const std::vector<Projectile>& getProjectiles() const { return projectiles; }

private:
    WeaponType equipped = WeaponType::Sword;
    WeaponType meleeWeapon = WeaponType::Sword;
    WeaponType rangedWeapon = WeaponType::Pistol;
    WeaponType swingWeapon = WeaponType::Sword;
    float swingDamage = 0;
    float muzzleFlash = 0;
    float meleeCooldown = 0;
    float rangedCooldown = 0;
    float swingRemaining = 0;
    unsigned int swingNumber = 0;
    Vector2 swingPosition = {};
    Vector2 swingDirection = {1, 0};
    std::vector<Projectile> projectiles;
    void attack(Player& player);
    void resolveSwing(Room& room);
};
