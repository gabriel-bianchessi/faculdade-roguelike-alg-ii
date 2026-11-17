#include "Enemy.h"

#include "Player.h"
#include "Room.h"
#include "raymath.h"

#include <algorithm>

Rectangle Enemy::getBounds() const
{
    const float half = type == EnemyType::Boss ? Config::bossHalfSize : Config::enemyHalfSize;
    return {position.x - half, position.y - half, half * 2, half * 2};
}

void Enemy::takeDamage(float damage)
{
    health = std::max(0.0f, health - damage);
    hitFlash = 0.12f;
}

void Enemy::takeMeleeDamage(float damage, unsigned int swing)
{
    if (lastHitSwing == swing) return;
    lastHitSwing = swing;
    takeDamage(damage);
}

void Enemy::update(float deltaTime, Room& room, Player& player)
{
    if (!isAlive()) return;
    hitFlash = std::max(0.0f, hitFlash-deltaTime);
    if (type == EnemyType::Boss) { updateBoss(deltaTime, room, player); return; }
    attackCooldown = std::max(0.0f, attackCooldown - deltaTime);
    const Vector2 difference = Vector2Subtract(player.getPosition(), position);
    const float reach = Config::enemyHalfSize + Config::playerHalfSize + 4;
    const float distance = Vector2Length(difference);
    const bool aware = distance <= Config::perceptionRadius;
    wanderTimer -= deltaTime;
    if (!aware && wanderTimer <= 0)
    {
        // Escolhas variadas e reproduziveis a partir da posicao, sem buscar caminhos a cada quadro.
        ++wanderStep;
        const unsigned int seed = static_cast<unsigned int>(position.x*17 + position.y*31) + wanderStep*7919;
        for (int attempt = 0; attempt < 24; ++attempt)
        {
            const unsigned int value = seed + attempt*104729u;
            const int x = value % room.getColumns(), y = (value/room.getColumns()) % room.getRows();
            if (room.isWalkable(x,y)) { wanderTarget = {(x+0.5f)*Config::cellSize,(y+0.5f)*Config::cellSize}; break; }
        }
        wanderTimer = Config::wanderInterval;
    }
    const Vector2 target = aware ? player.getPosition() : wanderTarget;
    const float speed = type == EnemyType::Patrol ? Config::patrolSpeed :
        type == EnemyType::Shooter ? Config::shooterSpeed : Config::enemySpeed;
    pathCooldown -= deltaTime;
    if (!aware || distance > reach)
    {
        Vector2 destination = target;
        if (!room.lineClear(position, target, Config::enemyHalfSize))
        {
            if (pathCooldown <= 0)
            {
                path = room.findPath(position, target);
                waypoint = 0;
                pathCooldown = Config::pathInterval;
            }
            while (waypoint < path.size() && Vector2Distance(position, path[waypoint]) < 3) ++waypoint;
            if (waypoint < path.size()) destination = path[waypoint];
        }
        const Vector2 offset = Vector2Subtract(destination, position);
        position = room.move(position, Vector2Scale(Vector2Normalize(offset),
            std::min(speed * deltaTime, Vector2Length(offset))), Config::enemyHalfSize);
    }
    const Vector2 firingDirection = Vector2Subtract(player.getPosition(),position);
    const bool shooting = type == EnemyType::Shooter && aware &&
        Vector2Length(firingDirection) < Config::shooterRange && room.lineClear(position,player.getPosition());
    if (aware && attackCooldown <= 0)
    {
        if (shooting)
        {
            room.getEnemyShots().push_back({position, Vector2Scale(Vector2Normalize(firingDirection), Config::hostileShotSpeed)});
            attackCooldown = Config::shooterInterval;
        }
        else if (Vector2Distance(position, player.getPosition()) <= reach)
        {
            player.takeDamage(Config::enemyDamage);
            attackCooldown = type == EnemyType::Patrol ? Config::patrolInterval : Config::enemyAttackInterval;
        }
    }
    if (attackCooldown <= 0)
        for (std::size_t i = 0; i < room.getBoxes().size(); ++i)
        {
            const Box& box = room.getBoxes()[i];
            const Vector2 nearest{std::clamp(position.x, box.bounds.x, box.bounds.x + box.bounds.width),
                std::clamp(position.y, box.bounds.y, box.bounds.y + box.bounds.height)};
            if (box.health > 0 && Vector2Distance(position, nearest) <= Config::enemyHalfSize + 6)
            {
                room.damageBox(i, Config::enemyDamage);
                attackCooldown = Config::enemyAttackInterval;
                break;
            }
        }
}

void Enemy::draw() const
{
    if (type == EnemyType::Boss)
    {
        const bool warning = bossState == BossState::ChargeWarning || bossState == BossState::FanWarning;
        DrawPoly(position, 6, 34, 30, warning ? GOLD : DARKPURPLE);
        DrawPolyLinesEx(position, 6, 34, 30, 3, MAGENTA);
        if (bossState == BossState::ChargeWarning)
            DrawLineEx(position, Vector2Add(position, Vector2Scale(chargeDirection, 300)), 10, Fade(RED, 0.5f));
        if (bossState == BossState::FanWarning) DrawCircleLines(position.x, position.y, 80, GOLD);
    }
    else if (type == EnemyType::Shooter) DrawCircleV(position, Config::enemyHalfSize, ORANGE);
    else if (type == EnemyType::Patrol) DrawPoly(position, 3, 21, 0, VIOLET);
    else DrawRectangleRec(getBounds(), MAROON);
    if (hitFlash > 0) DrawRectangleLinesEx(getBounds(), 3, WHITE);
    const int barWidth = type == EnemyType::Boss ? 72 : 32;
    const int barY = static_cast<int>(position.y) - (type == EnemyType::Boss ? 46 : 24);
    DrawRectangle(static_cast<int>(position.x)-barWidth/2, barY, barWidth, 4, DARKGRAY);
    DrawRectangle(static_cast<int>(position.x)-barWidth/2, barY,
        barWidth*health/(type == EnemyType::Boss ? Config::bossHealth : Config::enemyHealth), 4, RED);
}

void Enemy::updateBoss(float deltaTime, Room& room, Player& player)
{
    attackCooldown = std::max(0.0f, attackCooldown-deltaTime);
    stateTimer -= deltaTime;
    const bool enraged = health < Config::bossHealth/2;
    const Vector2 toward = Vector2Normalize(Vector2Subtract(player.getPosition(), position));
    if (bossState == BossState::Hunt)
    {
        position = room.move(position, Vector2Scale(toward, Config::bossSpeed*deltaTime), Config::bossHalfSize);
        if (stateTimer <= 0)
        {
            bossState = attackCycle++%2 == 0 ? BossState::ChargeWarning : BossState::FanWarning;
            chargeDirection = toward;
            stateTimer = enraged ? Config::bossEnragedWarningDuration : Config::bossWarningDuration;
        }
    }
    else if (bossState == BossState::ChargeWarning && stateTimer <= 0)
    {
        bossState = BossState::Charge;
        stateTimer = Config::bossChargeDuration;
    }
    else if (bossState == BossState::Charge)
    {
        position = room.move(position, Vector2Scale(chargeDirection, Config::bossChargeSpeed*deltaTime), Config::bossHalfSize);
        if (stateTimer <= 0) { bossState = BossState::Recovery; stateTimer = enraged ? Config::bossEnragedRecoveryDuration : Config::bossRecoveryDuration; }
    }
    else if (bossState == BossState::FanWarning && stateTimer <= 0)
    {
        for (int i = 0; i < Config::bossShotCount; ++i)
            room.getEnemyShots().push_back({position, Vector2Scale(Vector2Rotate(toward, (static_cast<float>(i)/(Config::bossShotCount-1)-0.5f)*Config::bossShotSpread),
                enraged ? Config::bossEnragedShotSpeed : Config::bossShotSpeed), Config::bossDamage});
        bossState = BossState::Recovery;
        stateTimer = enraged ? Config::bossEnragedRecoveryDuration : Config::bossRecoveryDuration;
    }
    else if (bossState == BossState::Recovery && stateTimer <= 0)
    {
        bossState = BossState::Hunt;
        stateTimer = enraged ? Config::bossEnragedHuntDuration : Config::bossHuntDuration;
    }
    if (attackCooldown <= 0 && Vector2Distance(position, player.getPosition()) <= Config::bossHalfSize+Config::playerHalfSize+4)
    {
        player.takeDamage(Config::bossDamage);
        attackCooldown = Config::bossAttackInterval;
    }
    for (std::size_t i = 0; i < room.getBoxes().size(); ++i)
        if (CheckCollisionRecs(getBounds(), room.getBoxes()[i].bounds)) room.damageBox(i, Config::bossDamage);
}
