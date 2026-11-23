#include "Game.h"
#include <cassert>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <unistd.h>

int main()
{
    const std::string directory="/tmp/rogue-ranking-"+std::to_string(getpid());
    const std::string path=directory+"/ranking.txt";
    assert(loadRanking(path).empty());
    assert(saveResult(path,{100,50,Difficulty::Easy,false}));
    assert(saveResult(path,{400,80,Difficulty::Hard,true}));
    assert(saveResult(path,{400,40,Difficulty::Normal,true}));
    { std::ofstream corrupt(path,std::ios::app); corrupt<<"bad\n-1 50 0 1\n20 4 9 1\n2 -1 1 0\n4 2 1 8\n1 2 0 1 garbage\n"; }
    auto results=loadRanking(path);
    assert(results.size()==3 && results[0].seconds==40 && results[1].score==400 && results[2].score==100);
    assert(!saveResult("/dev/null/ranking.txt",{100,0,Difficulty::Easy,false}));
    Player player;
    assert(player.getScore(2,9)==400 && player.getScore(2,10)==399);
    assert(player.getScore(0,10000)==0);
    Game game;
    game.rankingPath=path;
    game.startNewGame();
    game.player.recordVictory();
    game.elapsedTime=123;
    game.changeState(GameState::Victory);
    game.changeState(GameState::Victory);
    assert(loadRanking(path).size()==4);
    Game reopened;
    reopened.rankingPath=path;
    reopened.selectedOption=2;
    reopened.input.confirm=true;
    reopened.update(0);
    assert(reopened.state==GameState::Ranking && reopened.ranking.size()==4);
    game.startNewGame();
    game.changeState(GameState::Defeat);
    assert(loadRanking(path).size()==5);
    std::filesystem::remove_all(directory);
    std::cout<<"OK: tempo, ranking persistente ordenado, corrupcao, erros e resultado unico.\n";
}
