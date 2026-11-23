#include "Game.h"
#include "raymath.h"
#include <cassert>
#include <iostream>

int main()
{
    Dungeon dungeon(123);
    assert(dungeon.rooms.size()==13 && !dungeon.isSecretOpen());
    assert(dungeon.findRoom(13)->getConnections().empty());
    assert(dungeon.findRoom(13)->getEnemies().empty());
    assert(dungeon.findRoom(1)->getStage()==1 && dungeon.findRoom(4)->getStage()==2 && dungeon.findRoom(12)->getStage()==3);
    assert(!dungeon.interactSecret(dungeon.getSecretPosition()));
    dungeon.currentIndex=9;
    Room& source=dungeon.currentRoom();
    const auto oldCount=source.getConnections().size();
    const Vector2 secret=dungeon.getSecretPosition();
    assert(!dungeon.interactSecret({200,200}));
    assert(dungeon.interactSecret({secret.x-35,secret.y}));
    assert(dungeon.isSecretOpen() && source.getConnections().size()==oldCount+1);
    assert(!dungeon.interactSecret(secret));
    const Connection passage=source.getConnections().back();
    assert(source.freePosition({passage.door.x+24,passage.door.y+32},14));
    source.getEnemies().emplace_back(Vector2{200,300});
    Player player(secret);
    dungeon.update(.01f,player);
    assert(dungeon.currentRoom().getId()==10); // Combate ainda bloqueia o portal secreto.
    source.getEnemies().clear();
    dungeon.doorArmed=true;
    assert(dungeon.update(.01f,player) && dungeon.currentRoom().getId()==13);
    assert(dungeon.currentRoom().freePosition(player.getPosition(),14));
    Combat combat;
    player.enterRoom({512,440});
    collectItems(dungeon.currentRoom(),player,combat,false);
    assert(player.getLevel()==2 && player.getExperience()==50 && player.hasCollected(7));
    const auto remaining=dungeon.currentRoom().getItems().size();
    dungeon.currentIndex=9;
    dungeon.currentIndex=12;
    assert(dungeon.currentRoom().getItems().size()==remaining && dungeon.isSecretOpen());
    Game game;
    game.startNewGame();
    assert(!game.dungeon.isSecretOpen());
    std::cout<<"OK: passagem escondida, combate bloqueia entrada, tesouro/XP persistente e tres fases.\n";
}
