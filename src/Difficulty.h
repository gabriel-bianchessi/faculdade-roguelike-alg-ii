#pragma once

enum class Difficulty { Easy, Normal, Hard };

struct DifficultyRules
{
    int extraEnemies;
    int resourceChance;
    int chaserChance;
    int shooterChance;
};

inline DifficultyRules difficultyRules(Difficulty difficulty)
{
    switch (difficulty)
    {
        case Difficulty::Easy: return {-1, 75, 70, 20};
        case Difficulty::Normal: return {0, 50, 34, 33};
        case Difficulty::Hard: return {1, 25, 20, 40};
    }
    return {0, 50, 34, 33};
}

inline const char* difficultyName(Difficulty difficulty)
{
    switch (difficulty)
    {
        case Difficulty::Easy: return "Facil";
        case Difficulty::Normal: return "Normal";
        case Difficulty::Hard: return "Dificil";
    }
    return "Normal";
}
