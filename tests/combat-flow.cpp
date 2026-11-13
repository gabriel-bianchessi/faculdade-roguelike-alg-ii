// Compilar com -fno-access-control para verificar o fluxo interno sem abrir janela.
#include "Game.h"

#include <cassert>
#include <iostream>

int main()
{
    Game game;
    game.startNewGame();
    assert(game.state == GameState::Playing);
    assert(game.combat.getEnemies().size() == 1);
    game.input.switchWeapon = true;
    game.input.attack = true;
    game.update(0.01f);
    game.input = {};
    assert(game.combat.getWeapon().type == WeaponType::Pistol);
    const auto position = game.combat.getEnemies()[0].getPosition();
    const float time = game.elapsedTime;
    const auto shots = game.combat.getProjectiles().size();
    game.input.escape = true;
    game.update(0.01f);
    game.input = {};
    game.update(5);
    assert(game.state == GameState::Paused);
    assert(game.elapsedTime == time);
    assert(game.combat.getEnemies()[0].getPosition().x == position.x);
    assert(game.combat.getEnemies()[0].getPosition().y == position.y);
    assert(game.combat.getProjectiles().size() == shots);
    game.input.escape = true;
    game.update(0);
    game.input = {};
    assert(game.state == GameState::Playing);
    game.update(20); // O perseguidor chega, ataca e causa derrota.
    assert(game.state == GameState::Defeat);
    assert(game.player.getHealth() == 0);
    game.input.confirm = true;
    game.update(0);
    game.input = {};
    assert(game.state == GameState::Playing);
    assert(game.player.getHealth() == 100);
    assert(!game.player.isInvulnerable());
    assert(game.player.getDashCooldown() == 0);
    assert(game.player.getPosition().x == game.room.getSpawn().x);
    assert(game.elapsedTime == 0);
    assert(game.combat.getWeapon().type == WeaponType::Sword);
    assert(game.combat.getEnemies().size() == 1);
    assert(game.combat.getEnemies()[0].getHealth() == Config::enemyHealth);
    assert(game.combat.getProjectiles().empty());
    std::cout << "OK: pausa congela combate, derrota e reinicio com estado limpo.\n";
}
