// Compilar com -fno-access-control para verificar o fluxo interno sem abrir janela.
#include "Game.h"

#include <cassert>
#include <iostream>

int main()
{
    // Hover nao substitui a selecao do teclado; clique seleciona e confirma.
    for (GameState state : {GameState::Menu, GameState::Paused, GameState::Attributes,
                            GameState::Victory, GameState::Defeat})
    {
        Game menu;
        menu.changeState(state);
        const Rectangle first = menu.optionBounds(0), second = menu.optionBounds(1);
        menu.input.mouse = {first.x+10,first.y+10};
        menu.input.down = true;
        assert(menu.readMenuChoice(2) == -1);
        assert(menu.selectedOption == 1);
        menu.input.down = false;
        menu.input.mouse.x += 5; // Movimento sobre outra opcao nao altera o destaque.
        assert(menu.readMenuChoice(2) == -1 && menu.selectedOption == 1);
        menu.input.confirm = true;
        assert(menu.readMenuChoice(2) == 1);
        menu.input = {};
        menu.input.click = true;
        menu.input.mouse = {first.x+10,first.y+10};
        assert(menu.readMenuChoice(2) == 0 && menu.selectedOption == 0);
        menu.input.mouse = {second.x+10,second.y+10};
        assert(menu.readMenuChoice(2) == 1 && menu.selectedOption == 1);
    }
    Game game;
    game.startNewGame();
    game.dungeon.currentRoom().getEnemies().emplace_back(Vector2{320, 300});
    assert(game.state == GameState::Playing);
    assert(game.dungeon.currentRoom().getEnemies().size() == 1);
    game.input.switchWeapon = true;
    game.input.attack = true;
    game.update(0.01f);
    game.input = {};
    assert(game.combat.getWeapon().type == WeaponType::Pistol);
    game.player.boostDamage(0.25f, 10);
    game.player.grantShield(30);
    const float bonusTime = game.player.getBonusRemaining();
    const auto position = game.dungeon.currentRoom().getEnemies()[0].getPosition();
    const float time = game.elapsedTime;
    const auto shots = game.combat.getProjectiles().size();
    game.input.escape = true;
    game.update(0.01f);
    game.input = {};
    game.update(5);
    assert(game.state == GameState::Paused);
    assert(game.elapsedTime == time);
    assert(game.player.getBonusRemaining() == bonusTime);
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
    assert(game.player.getHealth() == 60);
    assert(!game.player.isInvulnerable());
    assert(game.player.getDashCooldown() == 0);
    assert(game.player.getShield() == 0);
    assert(game.player.getBonusRemaining() == 0);
    assert(game.player.getItemsCollected() == 0);
    assert(game.player.getPosition().x == game.dungeon.currentRoom().getSpawn().x);
    assert(game.elapsedTime == 0);
    assert(game.combat.getWeapon().type == WeaponType::Sword);
    game.dungeon.currentRoom().getEnemies().emplace_back(Vector2{320, 300});
    assert(game.dungeon.currentRoom().getEnemies().size() == 1);
    assert(game.dungeon.currentRoom().getEnemies()[0].getHealth() == Config::enemyHealth);
    assert(game.combat.getProjectiles().empty());
    game.combat.switchWeapon();
    game.combat.update(0, true, game.player, game.dungeon.currentRoom());
    assert(!game.combat.getProjectiles().empty());
    game.dungeon.currentRoom().getEnemies().clear(); // Portas exigem sala sem inimigos vivos.
    const Connection connection = game.dungeon.currentRoom().getConnections()[0];
    game.player.enterRoom({connection.door.x + connection.door.width / 2,
                           connection.door.y + connection.door.height / 2});
    game.update(0.01f);
    assert(game.dungeon.currentRoom().getId() == 2);
    assert(game.combat.getProjectiles().empty());
    assert(game.combat.getWeapon().type == WeaponType::Pistol);
    game.startNewGame();
    game.dungeon.currentRoom().getEnemies().emplace_back(Vector2{320, 300});
    assert(game.dungeon.getDiscoveredCount() == 1);
    assert(!game.dungeon.hasKey());
    game.dungeon.currentRoom().getItems().push_back({ItemType::Weapon, game.player.getPosition(), WeaponType::Spear});
    game.dungeon.currentRoom().getItems().push_back({ItemType::Weapon, game.player.getPosition(), WeaponType::Shotgun});
    game.input.interact = true;
    game.update(0.2f); // Varios passos internos, mas apenas uma interacao E.
    game.input = {};
    assert(game.player.getItemsCollected() == 1);
    assert(game.combat.getMeleeWeapon() == WeaponType::Sword);
    assert(game.combat.getRangedWeapon() == WeaponType::Shotgun);
    // O movimento continua apos o atalho sem exigir que a tecla seja solta.
    game.startNewGame();
    game.dungeon.currentIndex = 7; // Sala segura 8, com a curandeira.
    game.dungeon.finishDialogue(game.player);
    const Connection shortcut = game.dungeon.currentRoom().getConnections().back();
    assert(shortcut.to == 4);
    game.player.enterRoom({shortcut.door.x + shortcut.door.width / 2,
                           shortcut.door.y + shortcut.door.height / 2});
    game.input.movement = {1, 0};
    game.input.dash = true;
    game.update(1.0f / 240);
    assert(game.dungeon.currentRoom().getId() == 4);
    assert(game.player.getPosition().x == shortcut.arrival.x);
    assert(game.player.getPosition().y == shortcut.arrival.y);
    game.dungeon.currentRoom().getEnemies().clear();
    game.dungeon.currentRoom().configureWaves({}, {});
    game.input.dash = false;
    game.update(0.05f); // Mantem a mesma direcao pressionada.
    assert(game.dungeon.currentRoom().getId() == 4);
    assert(game.player.getPosition().x > shortcut.arrival.x);
    assert(game.player.getPosition().y == shortcut.arrival.y);
    std::cout << "OK: pausa congela combate, derrota e reinicio com estado limpo.\n";
}
