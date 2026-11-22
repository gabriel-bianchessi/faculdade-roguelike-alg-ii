#include "Game.h"
#include <cassert>
#include <cmath>
#include <iostream>

int main()
{
    Room room(5000,5000);
    room.setObstacles({});
    Player player({300,300});
    Combat combat;
    combat.reload(); // Espada nao possui pente.
    assert(combat.getAmmo() == 0 && combat.getReloadRemaining() == 0);
    combat.switchWeapon();
    assert(combat.getAmmo() == 15);
    combat.reload();
    assert(combat.getReloadRemaining() == 0); // Nao recarrega pente cheio.
    for (int shot=0;shot<15;++shot)
    {
        combat.update(shot == 0 ? 0 : 0.3f,true,player,room);
        assert(combat.getAmmo() == 14-shot);
    }
    assert(combat.getReloadRemaining() == reloadDuration(WeaponType::Pistol));
    combat.clearTransient(); // Trocar de sala nao reabastece.
    combat.update(0.3f,true,player,room);
    assert(combat.getAmmo() == 0 && combat.getProjectiles().empty());
    combat.update(0.91f,false,player,room);
    assert(combat.getAmmo() == 15 && combat.getReloadRemaining() == 0);
    combat.update(0,true,player,room);
    combat.reload();
    const float remaining = combat.getReloadRemaining();
    combat.reload(); // R repetido nao reinicia o temporizador.
    assert(combat.getReloadRemaining() == remaining);
    combat.switchWeapon();
    combat.update(1.21f,false,player,room);
    combat.switchWeapon();
    assert(combat.getAmmo() == 15); // Recarga continua com arma guardada.
    combat.update(0,true,player,room);
    combat.replaceWeapon(WeaponType::Shotgun);
    assert(combat.getAmmo() == 7);
    combat.clearTransient();
    combat.update(0.9f,true,player,room);
    assert(combat.getAmmo() == 6 && combat.getProjectiles().size() == 5);
    for (int shot=1;shot<7;++shot) combat.update(0.9f,true,player,room);
    assert(combat.getAmmo() == 0 && combat.getReloadRemaining() == 1.8f);
    combat.replaceWeapon(WeaponType::Pistol);
    assert(combat.getAmmo() == 14); // Cada tipo preserva seu pente.
    combat.replaceWeapon(WeaponType::Shotgun);
    assert(combat.getAmmo() == 0);
    combat.update(1.81f,false,player,room);
    assert(combat.getAmmo() == 7);

    Game game;
    game.startNewGame();
    game.combat.switchWeapon();
    game.combat.update(0,true,game.player,game.dungeon.currentRoom());
    game.input.reload = true;
    game.update(0.01f);
    assert(game.combat.getReloadRemaining() > 0);
    for (GameState state : {GameState::Paused,GameState::Attributes,GameState::Dialogue})
    {
        game.input = {};
        game.changeState(state);
        const float before = game.combat.getReloadRemaining();
        game.update(10);
        assert(game.combat.getReloadRemaining() == before);
    }
    game.startNewGame();
    game.combat.switchWeapon();
    assert(game.combat.getAmmo() == 15 && game.combat.getReloadRemaining() == 0);
    std::cout << "OK: pentes 15/7, cartucho unico por leque, recarga manual/automatica, reserva infinita, troca, pausa e reinicio.\n";
}
