#pragma once

#include "raylib.h"
#include <algorithm>

namespace Config
{
    inline constexpr int windowWidth = 960;
    inline constexpr int windowHeight = 640;
    inline float windowScale(int width, int height)
    {
        if (width <= 0 || height <= 0) return 1;
        return std::min(static_cast<float>(width)/windowWidth,static_cast<float>(height)/windowHeight);
    }
    inline constexpr int targetFps = 60;
    inline constexpr const char* windowTitle = "Rogue Like";

    inline constexpr float interactionRadius = 60;
    inline constexpr float boxHealth = 40;
    inline constexpr float trapRestDuration = 2;
    inline constexpr float trapWarningDuration = 1;
    inline constexpr float trapActiveDuration = 0.8f;
    inline constexpr float trapDamageInterval = 0.6f;
    inline constexpr int trapDamage = 15;
    inline constexpr float hostileShotLifetime = 3;
    inline constexpr float shooterRange = 360;
    inline constexpr float wanderInterval = 2.5f;
    inline constexpr float bossHuntDuration = 1.5f;
    inline constexpr float bossEnragedHuntDuration = 0.9f;
    inline constexpr float bossWarningDuration = 0.9f;
    inline constexpr float bossEnragedWarningDuration = 0.65f;
    inline constexpr float bossChargeDuration = 0.45f;
    inline constexpr float bossRecoveryDuration = 0.8f;
    inline constexpr float bossEnragedRecoveryDuration = 0.4f;
    inline constexpr float bossAttackInterval = 0.8f;
    inline constexpr float bossShotSpeed = 280;
    inline constexpr float bossEnragedShotSpeed = 340;
    inline constexpr int bossShotCount = 7;
    inline constexpr float bossShotSpread = 1.2f;
    inline constexpr int scoreEnemy = 100;
    inline constexpr int scoreItem = 50;
    inline constexpr int scoreRoom = 200;
    inline constexpr int scoreVictory = 1000;
    inline constexpr double scoreDistanceUnit = 100;
    inline constexpr int scoreHealingPenalty = 2;
    inline constexpr int commonExperience = 20;
    inline constexpr int bossExperience = 100;
    inline constexpr int levelCost = 50;
    inline constexpr float strengthBonus = 0.1f;
    inline constexpr int vitalityBonus = 20;
    inline constexpr float agilityBonus = 0.05f;
    inline constexpr int initialHealth = 60;
    inline constexpr float initialSpeed = 240.0f;
    inline constexpr float playerHalfSize = 14.0f;
    inline constexpr float aimLength = 34.0f;
    inline constexpr float roomWidth = 1280.0f;
    inline constexpr float roomHeight = 800.0f;
    inline constexpr float wallThickness = 32.0f;
    inline constexpr float itemRadius = 12.0f;
    inline constexpr float weaponPickupRadius = 44.0f;
    inline constexpr int kitHeal = 30;
    inline constexpr int shieldAmount = 30;
    inline constexpr int damageBonusPercent = 25;
    inline constexpr float damageBonusDuration = 10.0f;
    inline constexpr float cellSize = 32.0f;
    inline constexpr float revealRadius = 460.0f;
    inline constexpr float viewAngle = 220.0f;
    inline constexpr float nearVisionRadius = 110.0f;
    inline constexpr unsigned char exploredShadow = 125;
    inline constexpr unsigned char unexploredShadow = 205;
    inline constexpr float waveDelay = 3.0f;
    inline constexpr float waveSpawnDistance = 220.0f;
    inline constexpr float transitionCooldown = 0.3f;
    inline constexpr float cameraZoom = 1.0f;
    inline constexpr float swordActiveDuration = 0.12f;
    inline constexpr float projectileRadius = 4.0f;
    inline constexpr float dashDuration = 0.24f;
    inline constexpr float dashInvulnerability = 0.12f;
    inline constexpr float dashCooldown = 1.0f;
    inline constexpr float dashSpeed = 800.0f;
    inline constexpr float damageInvulnerability = 0.5f;
    inline constexpr int bossHealth = 400;
    inline constexpr float bossHalfSize = 28;
    inline constexpr float bossSpeed = 95;
    inline constexpr float bossChargeSpeed = 620;
    inline constexpr int bossDamage = 20;
    inline constexpr float perceptionRadius = 540;
    inline constexpr float pathInterval = 0.35f;
    inline constexpr float shooterSpeed = 90;
    inline constexpr float patrolSpeed = 180;
    inline constexpr float shooterInterval = 1.4f;
    inline constexpr float patrolInterval = 0.55f;
    inline constexpr float hostileShotSpeed = 300;
    inline constexpr int enemyHealth = 40;
    inline constexpr float enemySpeed = 120.0f;
    inline constexpr float enemyHalfSize = 16.0f;
    inline constexpr int enemyDamage = 10;
    inline constexpr float enemyAttackInterval = 0.8f;
    inline constexpr float hudHeight = 88.0f;
    inline constexpr float footerHeight = 48.0f;

    inline constexpr Color background = {18, 22, 30, 255};
    inline constexpr Color panel = {32, 39, 50, 255};
    inline constexpr Color text = {232, 237, 242, 255};
    inline constexpr Color muted = {154, 170, 185, 255};
    inline constexpr Color accent = {90, 205, 170, 255};
    inline constexpr Color overlay = {0, 0, 0, 190};
    inline constexpr Color floor = {26, 32, 41, 255};
    inline constexpr Color wall = {83, 99, 115, 255};
    inline constexpr Color obstacle = {115, 88, 65, 255};
}
