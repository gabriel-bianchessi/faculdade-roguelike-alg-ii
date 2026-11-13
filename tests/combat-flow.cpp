// Compilar com -fno-access-control para verificar o fluxo interno sem abrir janela.
#include "Game.h"

#include <cassert>
#include <iostream>

int main()
{
    Game game;
    game.startNewGame();
    assert(game.state == GameState::Playing);
    assert(game.dungeon.currentRoom().getEnemies().size() == 1);
    game.input.switchWeapon = true;
    game.input.attack = true;
    game.update(0.01f);
    game.input = {};
    assert(game.combat.getWeapon().type == WeaponType::Pistol);
    const auto position = game.dungeon.currentRoom().getEnemies()[0].getPosition();
    const float time = game.elapsedTime;
    const auto shots = game.combat.getProjectiles().size();
    game.input.escape = true;
    game.update(0.01f);
    game.input = {};
    game.update(5);
    assert(game.state == GameState::Paused);
    assert(game.elapsedTime == time);
    assert(game.dungeon.currentRoom().getEnemies()[0].getPosition().x == position.x);
    assert(game.dungeon.currentRoom().getEnemies()[0].getPosition().y == position.y);
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
    assert(game.player.getPosition().x == game.dungeon.currentRoom().getSpawn().x);
    assert(game.elapsedTime == 0);
    assert(game.combat.getWeapon().type == WeaponType::Sword);
    assert(game.dungeon.currentRoom().getEnemies().size() == 1);
    assert(game.dungeon.currentRoom().getEnemies()[0].getHealth() == Config::enemyHealth);
    assert(game.combat.getProjectiles().empty());
    game.combat.switchWeapon();
    game.combat.update(0, true, game.player, game.dungeon.currentRoom());
    assert(!game.combat.getProjectiles().empty());
    const Connection connection = game.dungeon.currentRoom().getConnections()[0];
    game.player.enterRoom({connection.door.x + connection.door.width / 2,
                           connection.door.y + connection.door.height / 2});
    game.update(0.01f);
    assert(game.dungeon.currentRoom().getId() == 2);
    assert(game.combat.getProjectiles().empty());
    assert(game.combat.getWeapon().type == WeaponType::Pistol);
    game.startNewGame();
    assert(game.dungeon.getDiscoveredCount() == 1);
    assert(!game.dungeon.hasKey());
    std::cout << "OK: pausa congela combate, derrota e reinicio com estado limpo.\n";
}
