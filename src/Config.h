#pragma once

#include "raylib.h"

namespace Config
{
    inline constexpr int windowWidth = 960;
    inline constexpr int windowHeight = 640;
    inline constexpr int targetFps = 60;
    inline constexpr const char* windowTitle = "Rogue Like";

    inline constexpr int initialHealth = 100;
    inline constexpr float initialSpeed = 180.0f;

    inline constexpr Color background = {18, 22, 30, 255};
    inline constexpr Color panel = {32, 39, 50, 255};
    inline constexpr Color text = {232, 237, 242, 255};
    inline constexpr Color muted = {154, 170, 185, 255};
    inline constexpr Color accent = {90, 205, 170, 255};
    inline constexpr Color overlay = {0, 0, 0, 190};
}
