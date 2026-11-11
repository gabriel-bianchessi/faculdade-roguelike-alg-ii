#pragma once

#include "raylib.h"

namespace Config
{
    inline constexpr int windowWidth = 960;
    inline constexpr int windowHeight = 640;
    inline constexpr int targetFps = 60;
    inline constexpr const char* windowTitle = "Rogue Like";

    inline constexpr int initialHealth = 100;
    inline constexpr float initialSpeed = 240.0f;
    inline constexpr float playerHalfSize = 14.0f;
    inline constexpr float aimLength = 34.0f;
    inline constexpr float roomWidth = 1280.0f;
    inline constexpr float roomHeight = 800.0f;
    inline constexpr float wallThickness = 32.0f;
    inline constexpr float cameraZoom = 1.2f;
    inline constexpr float hudHeight = 64.0f;
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
