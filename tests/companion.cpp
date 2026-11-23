#include "Game.h"
#include "raymath.h"
#include <cassert>
#include <iostream>

int main()
{
    Room room;
    room.setObstacles({{500,200,80,300}});
    Player player({900,400});
    Companion companion;
    companion.recruit({200,400});
    for (int i=0; i<1200; ++i)
    {
        companion.update(1.0f/120,player,room);
        assert(room.freePosition(companion.getPosition(),12));
    }
    assert(Vector2Distance(companion.getPosition(),player.getPosition())<73);
    companion.enterRoom({200,400});
    player.enterRoom({594,400}); // O jogador cabe, mas a celula de destino da grade nao.
    assert(!room.isWalkable(18,12));
    for (int i=0; i<1200; ++i)
    {
        companion.update(1.0f/120,player,room);
        assert(room.freePosition(companion.getPosition(),12));
    }
    assert(Vector2Distance(companion.getPosition(),player.getPosition())<73);
    companion.enterRoom({300,500});
    player.enterRoom({300,500});
    room.getEnemies().emplace_back(Vector2{340,500});
    Combat combat;
    for (int i=0; i<600; ++i)
    {
        companion.update(.01f,player,room);
        combat.update(.01f,false,player,room);
    }
    assert(room.getEnemies().empty() && player.getDefeatedCount()==1 && player.getExperience()==20);
    combat.update(0,false,player,room);
    assert(player.getDefeatedCount()==1);
    Room arena(1280,800,12);
    arena.setObstacles({});
    arena.getEnemies().emplace_back(Vector2{340,500},EnemyType::Boss);
    arena.getEnemies()[0].takeDamage(Config::bossHealth-8);
    companion.update(1,player,arena);
    combat.update(0,false,player,arena);
    assert(player.hasWon() && player.getDefeatedCount()==2 && arena.getDyingEnemies().size()==1);
    Game game;
    game.startNewGame();
    game.player.enterRoom(game.dungeon.currentRoom().getNpc().position);
    game.input.interact=true;
    game.update(.1f);
    for (int i=0; i<3; ++i) game.update(.1f);
    assert(game.companion.isActive());
    game.input={};
    const Vector2 before=game.companion.getPosition();
    game.changeState(GameState::Paused);
    game.update(5);
    assert(Vector2Distance(before,game.companion.getPosition())==0);
    game.changeState(GameState::Playing);
    const auto door=game.dungeon.currentRoom().getConnections()[0].door;
    game.player.enterRoom({door.x+door.width/2,door.y+door.height/2});
    game.update(.01f);
    assert(game.dungeon.currentRoom().getId()==2);
    assert(Vector2Distance(game.companion.getPosition(),game.player.getPosition())<2);
    game.startNewGame();
    assert(!game.companion.isActive());
    std::cout<<"OK: recrutamento, navegacao, colisao, ajuda/XP unicos, pausa, transicao e reinicio.\n";
}
