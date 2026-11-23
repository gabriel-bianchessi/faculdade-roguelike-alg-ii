#pragma once

#include "Difficulty.h"
#include <string>
#include <vector>

struct Result
{
    int score;
    float seconds;
    Difficulty difficulty;
    bool victory;
};

std::string defaultRankingPath();
std::vector<Result> loadRanking(const std::string& path);
bool saveResult(const std::string& path, const Result& result);
