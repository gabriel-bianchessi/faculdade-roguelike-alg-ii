#include "Ranking.h"

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <sstream>

std::string defaultRankingPath()
{
    if (const char* data = std::getenv("XDG_DATA_HOME"); data && *data)
        return std::string(data)+"/rogue-like/ranking.txt";
    if (const char* user = std::getenv("HOME"); user && *user)
        return std::string(user)+"/.local/share/rogue-like/ranking.txt";
    return "ranking.txt";
}

std::vector<Result> loadRanking(const std::string& path)
{
    std::ifstream file(path);
    std::vector<Result> results;
    std::string line;
    while (std::getline(file,line))
    {
        std::istringstream row(line);
        Result result{};
        int difficulty, victory;
        std::string extra;
        if (!(row >> result.score >> result.seconds >> difficulty >> victory) || row >> extra) continue;
        if (result.score < 0 || !std::isfinite(result.seconds) || result.seconds < 0 ||
            difficulty < 0 || difficulty > 2 || victory < 0 || victory > 1) continue;
        result.difficulty = static_cast<Difficulty>(difficulty);
        result.victory = victory == 1;
        results.push_back(result);
    }
    std::stable_sort(results.begin(),results.end(),[](const Result& a, const Result& b) {
        return a.score != b.score ? a.score > b.score : a.seconds < b.seconds;
    });
    return results;
}

bool saveResult(const std::string& path, const Result& result)
{
    const auto directory = std::filesystem::path(path).parent_path();
    std::error_code error;
    if (!directory.empty()) std::filesystem::create_directories(directory,error);
    if (error) return false;
    std::ofstream file(path,std::ios::app);
    file << result.score << ' ' << result.seconds << ' ' << static_cast<int>(result.difficulty)
         << ' ' << result.victory << '\n';
    file.flush();
    return static_cast<bool>(file);
}
