#include "Combat.h"
#include "Player.h"
#include "Room.h"
#include "raymath.h"
#include <algorithm>
#include <cmath>

namespace
{
    // Varredura: primeira fracao do segmento que toca a caixa.
    float hitFraction(Vector2 start, Vector2 end, Rectangle box, float radius = 0)
    {
        box.x -= radius; box.y -= radius;
        box.width += radius * 2; box.height += radius * 2;
        float enter = 0, leave = 1;
        const Vector2 delta = Vector2Subtract(end, start);
        const float origins[] = {start.x, start.y};
        const float directions[] = {delta.x, delta.y};
        const float minima[] = {box.x, box.y};
        const float maxima[] = {box.x + box.width, box.y + box.height};
        for (int axis = 0; axis < 2; ++axis)
        {
            if (std::abs(directions[axis]) < 0.00001f)
            {
                if (origins[axis] < minima[axis] || origins[axis] > maxima[axis]) return 2;
                continue;
            }
            float first = (minima[axis] - origins[axis]) / directions[axis];
            float last = (maxima[axis] - origins[axis]) / directions[axis];
            if (first > last) std::swap(first, last);
            enter = std::max(enter, first);
            leave = std::min(leave, last);
            if (enter > leave) return 2;
        }
        return enter;
    }

    bool swordHits(Vector2 origin, Vector2 direction, Rectangle target)
    {
        const Vector2 side{-direction.y, direction.x};
        const Vector2 center = Vector2Add(origin, Vector2Scale(direction, sword.reach / 2));
        const Vector2 difference = Vector2Subtract(
            {target.x + target.width / 2, target.y + target.height / 2}, center);
        const Vector2 axes[] = {direction, side, {1, 0}, {0, 1}};
        for (Vector2 axis : axes)
        {
            const float attackRadius = std::abs(Vector2DotProduct(axis, direction)) * sword.reach / 2 +
                                       std::abs(Vector2DotProduct(axis, side)) * sword.width / 2;
            const float targetRadius = std::abs(axis.x) * target.width / 2 + std::abs(axis.y) * target.height / 2;
            if (std::abs(Vector2DotProduct(difference, axis)) > attackRadius + targetRadius) return false;
        }
        return true;
    }
}

void Combat::switchWeapon()
{
    equipped = equipped == WeaponType::Sword ? WeaponType::Pistol : WeaponType::Sword;
}

void Combat::attack(const Player& player)
{
    const WeaponConfig& weapon = getWeapon();
    float& cooldown = equipped == WeaponType::Sword ? swordCooldown : pistolCooldown;
    if (cooldown > 0.00001f) return;
    cooldown += weapon.interval;
    if (weapon.category == WeaponCategory::Ranged)
    {
        projectiles.push_back({player.getPosition(), Vector2Scale(player.getDirection(), weapon.projectileSpeed),
                              weapon.damage, weapon.projectileLifetime});
        return;
    }
    swingRemaining = Config::swordActiveDuration;
    swingPosition = player.getPosition();
    swingDirection = player.getDirection();
    ++swingNumber;
}

void Combat::resolveSwing(Room& room)
{
    // Cada inimigo registra o golpe recebido, inclusive durante os quadros da animacao.
    for (Enemy& enemy : room.getEnemies())
    {
        if (!enemy.isAlive() || !swordHits(swingPosition, swingDirection, enemy.getBounds())) continue;
        bool blocked = false;
        for (const Rectangle& solid : room.getSolids())
            if (hitFraction(swingPosition, enemy.getPosition(), solid) <= 1) blocked = true;
        if (!blocked) enemy.takeMeleeDamage(sword.damage, swingNumber);
    }
}

void Combat::update(float deltaTime, bool attacking, const Player& player, Room& room)
{
    swordCooldown -= deltaTime;
    pistolCooldown -= deltaTime;
    swingRemaining = std::max(0.0f, swingRemaining - deltaTime);
    if (attacking) attack(player);
    if (swingRemaining > 0) resolveSwing(room);
    swordCooldown = std::max(0.0f, swordCooldown);
    pistolCooldown = std::max(0.0f, pistolCooldown);
    for (Projectile& projectile : projectiles)
    {
        const Vector2 end = Vector2Add(projectile.position,
            Vector2Scale(projectile.velocity, std::min(deltaTime, projectile.remaining)));
        float nearest = 2;
        Enemy* target = nullptr;
        for (const Rectangle& solid : room.getSolids())
            nearest = std::min(nearest, hitFraction(projectile.position, end, solid, Config::projectileRadius));
        for (Enemy& enemy : room.getEnemies())
        {
            if (!enemy.isAlive()) continue;
            const float fraction = hitFraction(projectile.position, end, enemy.getBounds(), Config::projectileRadius);
            if (fraction < nearest)
            {
                nearest = fraction;
                target = &enemy;
            }
        }
        projectile.position = Vector2Lerp(projectile.position, end, std::min(1.0f, nearest));
        projectile.remaining -= deltaTime;
        if (nearest <= 1)
        {
            if (target) target->takeDamage(projectile.damage);
            projectile.remaining = 0;
        }
    }
    std::erase_if(projectiles, [](const Projectile& projectile) { return projectile.remaining <= 0; });
    std::erase_if(room.getEnemies(), [](const Enemy& enemy) { return !enemy.isAlive(); });
}

void Combat::draw(const Player& player, const Room& room) const
{
    for (const Enemy& enemy : room.getEnemies())
        if (room.isVisible(enemy.getPosition())) enemy.draw();
    for (const Projectile& projectile : projectiles) DrawCircleV(projectile.position, Config::projectileRadius, GOLD);
    if (swingRemaining > 0)
    {
        const Vector2 side{-swingDirection.y, swingDirection.x};
        const Vector2 left = Vector2Add(swingPosition, Vector2Scale(side, sword.width / 2));
        const Vector2 right = Vector2Subtract(swingPosition, Vector2Scale(side, sword.width / 2));
        const Vector2 offset = Vector2Scale(swingDirection, sword.reach);
        DrawLineEx(left, Vector2Add(left, offset), 3, GOLD);
        DrawLineEx(Vector2Add(left, offset), Vector2Add(right, offset), 3, GOLD);
        DrawLineEx(Vector2Add(right, offset), right, 3, GOLD);
    }
    if (equipped == WeaponType::Pistol)
        DrawCircleV(Vector2Add(player.getPosition(), Vector2Scale(player.getDirection(), 22)), 5, GOLD);
}

void Combat::clearTransient()
{
    projectiles.clear();
    swingRemaining = 0;
}
