#include "Game.h"
#include <cassert>
#include <iostream>

int main()
{
    Room room;
    Player player({200,300});
    Combat combat;
    assert(player.hasCollected(3) && player.hasCollected(4));
    room.getItems().push_back({ItemType::Experience,player.getPosition(),WeaponType::Sword,30});
    collectItems(room,player,combat,false);
    assert(room.getItems().empty() && player.getExperience()==30 && player.hasCollected(7));
    room.getItems().push_back({ItemType::Weapon,player.getPosition(),WeaponType::Spear});
    collectItems(room,player,combat,true);
    assert(player.hasCollected(5) && !player.hasCollected(6));
    Game game;
    game.startNewGame();
    game.player.boostDamage(.25f,10);
    game.input.inventory=true;
    game.update(1);
    assert(game.state==GameState::Inventory && game.elapsedTime==0);
    game.input={};
    game.update(10);
    assert(game.player.getBonusRemaining()==10);
    game.input.escape=true;
    game.update(0);
    assert(game.state==GameState::Playing);
    game.startNewGame();
    assert(!game.player.hasCollected(7));
    std::cout<<"OK: cristal de XP, historico de coletas, inventario e pausa.\n";
}
