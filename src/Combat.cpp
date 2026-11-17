#include "Combat.h"
#include "Player.h"
#include "Room.h"
#include "raymath.h"
#include "Geometry.h"
#include "WeaponVisual.h"
#include <algorithm>
#include <cmath>

namespace
{

    bool meleeHits(Vector2 origin, Vector2 direction, Rectangle target, const WeaponConfig& weapon)
    {
        const Vector2 side{-direction.y, direction.x};
        const Vector2 center = Vector2Add(origin, Vector2Scale(direction, weapon.reach / 2));
        const Vector2 difference = Vector2Subtract(
            {target.x + target.width / 2, target.y + target.height / 2}, center);
        const Vector2 axes[] = {direction, side, {1, 0}, {0, 1}};
        for (Vector2 axis : axes)
        {
            const float attackRadius = std::abs(Vector2DotProduct(axis, direction)) * weapon.reach / 2 +
                                       std::abs(Vector2DotProduct(axis, side)) * weapon.width / 2;
            const float targetRadius = std::abs(axis.x) * target.width / 2 + std::abs(axis.y) * target.height / 2;
            if (std::abs(Vector2DotProduct(difference, axis)) > attackRadius + targetRadius) return false;
        }
        return true;
    }
}

void Combat::switchWeapon()
{
    equipped = getWeapon().category == WeaponCategory::Melee ? rangedWeapon : meleeWeapon;
}

WeaponType Combat::replaceWeapon(WeaponType type)
{
    WeaponType& slot = weaponConfig(type).category == WeaponCategory::Melee ? meleeWeapon : rangedWeapon;
    const WeaponType previous = slot;
    slot = type;
    equipped = type;
    return previous;
}

void Combat::attack(const Player& player)
{
    const WeaponConfig& weapon = getWeapon();
    float& cooldown = weapon.category == WeaponCategory::Melee ? meleeCooldown : rangedCooldown;
    if (cooldown > 0.00001f) return;
    cooldown += weapon.interval;
    if (weapon.category == WeaponCategory::Ranged)
    {
        muzzleFlash = 0.07f;
        for (int pellet = 0; pellet < weapon.projectileCount; ++pellet)
        {
            const float angle = weapon.projectileCount == 1 ? 0 :
                weapon.spread * (static_cast<float>(pellet) / (weapon.projectileCount - 1) - 0.5f);
            projectiles.push_back({player.getPosition(),
                Vector2Scale(Vector2Rotate(player.getDirection(), angle), weapon.projectileSpeed),
                weapon.damage * player.getDamageMultiplier(), weapon.projectileLifetime});
        }
        return;
    }
    swingRemaining = Config::swordActiveDuration;
    swingWeapon = equipped;
    swingDamage = weapon.damage * player.getDamageMultiplier();
    swingPosition = player.getPosition();
    swingDirection = player.getDirection();
    ++swingNumber;
}

void Combat::resolveSwing(Room& room)
{
    // Cada inimigo registra o golpe recebido, inclusive durante os quadros da animacao.
    for (Enemy& enemy : room.getEnemies())
    {
        if (!enemy.isAlive() || !meleeHits(swingPosition, swingDirection, enemy.getBounds(), weaponConfig(swingWeapon))) continue;
        bool blocked = false;
        for (const Rectangle& solid : room.getSolids())
            if (hitFraction(swingPosition, enemy.getPosition(), solid) <= 1) blocked = true;
        if (!blocked) enemy.takeMeleeDamage(swingDamage, swingNumber);
    }
    for (std::size_t i = 0; i < room.getBoxes().size(); ++i)
    {
        const Box box = room.getBoxes()[i];
        if (box.health <= 0 || !meleeHits(swingPosition, swingDirection, box.bounds, weaponConfig(swingWeapon))) continue;
        const Vector2 center{box.bounds.x + box.bounds.width/2, box.bounds.y + box.bounds.height/2};
        const float targetFraction = hitFraction(swingPosition, center, box.bounds);
        bool blocked = false;
        for (const Rectangle& solid : room.getSolids())
            if (hitFraction(swingPosition, center, solid) < targetFraction) blocked = true;
        if (!blocked) room.damageBox(i, swingDamage, swingNumber);
    }
}

void Combat::update(float deltaTime, bool attacking, Player& player, Room& room)
{
    muzzleFlash = std::max(0.0f,muzzleFlash-deltaTime);
    meleeCooldown -= deltaTime;
    rangedCooldown -= deltaTime;
    swingRemaining = std::max(0.0f, swingRemaining - deltaTime);
    if (attacking) attack(player);
    if (swingRemaining > 0) resolveSwing(room);
    meleeCooldown = std::max(0.0f, meleeCooldown);
    rangedCooldown = std::max(0.0f, rangedCooldown);
    for (Projectile& projectile : projectiles)
    {
        const Vector2 end = Vector2Add(projectile.position,
            Vector2Scale(projectile.velocity, std::min(deltaTime, projectile.remaining)));
        float nearest = 2;
        Enemy* target = nullptr;
        int targetBox = -1;
        for (const Rectangle& solid : room.getSolids())
            nearest = std::min(nearest, hitFraction(projectile.position, end, solid, Config::projectileRadius));
        for (std::size_t i = 0; i < room.getBoxes().size(); ++i)
        {
            const Box& box = room.getBoxes()[i];
            if (box.health <= 0) continue;
            const float fraction = hitFraction(projectile.position, end, box.bounds, Config::projectileRadius);
            if (fraction <= nearest) { nearest = fraction; targetBox = static_cast<int>(i); }
        }
        for (Enemy& enemy : room.getEnemies())
        {
            if (!enemy.isAlive()) continue;
            const float fraction = hitFraction(projectile.position, end, enemy.getBounds(), Config::projectileRadius);
            if (fraction < nearest)
            {
                nearest = fraction;
                target = &enemy;
                targetBox = -1;
            }
        }
        projectile.position = Vector2Lerp(projectile.position, end, std::min(1.0f, nearest));
        projectile.remaining -= deltaTime;
        if (nearest <= 1)
        {
            if (target) target->takeDamage(projectile.damage);
            if (targetBox >= 0) room.damageBox(targetBox, projectile.damage);
            projectile.remaining = 0;
        }
    }
    std::erase_if(projectiles, [](const Projectile& projectile) { return projectile.remaining <= 0; });
    std::erase_if(room.getEnemies(), [&player, &room](const Enemy& enemy) {
        if (enemy.isAlive()) return false;
        player.recordDefeat(enemy.getExperienceReward());
        if (enemy.getType() == EnemyType::Boss && room.getId() == 12) player.recordVictory();
        return true;
    });
}

void Combat::draw(const Player& player, const Room& room) const
{
    for (const Enemy& enemy : room.getEnemies())
        if (enemy.isAlive()) enemy.draw();
    for (const EnemyShot& shot : room.getEnemyShots()) DrawCircleV(shot.position, 4, RED);
    for (const Projectile& projectile : projectiles) DrawCircleV(projectile.position, Config::projectileRadius, GOLD);
    Vector2 direction = player.getDirection();
    const Vector2 side{-direction.y,direction.x};
    Vector2 grip = Vector2Add(player.getPosition(),Vector2Add(Vector2Scale(direction,12),Vector2Scale(side,7)));
    WeaponType drawn = equipped;
    if (swingRemaining > 0)
    {
        drawn = swingWeapon;
        const float progress = 1-swingRemaining/Config::swordActiveDuration;
        if (drawn == WeaponType::Sword)
        {
            direction = Vector2Rotate(swingDirection,-0.9f+progress*1.8f);
            const float angle = std::atan2(swingDirection.y,swingDirection.x)*RAD2DEG;
            DrawRing(swingPosition,48,64,angle-45,angle-45+progress*90,20,Fade(GOLD,0.23f));
        }
        else
        {
            direction = swingDirection;
            grip = Vector2Add(grip,Vector2Scale(direction,std::sin(progress*PI)*44));
            DrawLineEx(grip,Vector2Add(grip,Vector2Scale(direction,62)),3,Fade(GOLD,0.35f));
        }
    }
    if (muzzleFlash > 0) grip = Vector2Subtract(grip,Vector2Scale(direction,3));
    drawWeapon(drawn,grip,direction);
    if (muzzleFlash > 0 && weaponConfig(drawn).category == WeaponCategory::Ranged)
    {
        const float length = drawn == WeaponType::Shotgun ? 48 : 32;
        const Vector2 tip = Vector2Add(grip,Vector2Scale(direction,length));
        DrawPoly(tip,4,10,45,Fade(GOLD,muzzleFlash/0.07f));
        DrawCircleV(tip,3,WHITE);
    }
}

void Combat::clearTransient()
{
    projectiles.clear();
    swingRemaining = 0;
    muzzleFlash = 0;
}
