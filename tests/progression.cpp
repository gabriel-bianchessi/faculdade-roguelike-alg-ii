#include "Game.h"
#include "raymath.h"
#include <cassert>
#include <cmath>
#include <iostream>

int main()
{
    Player player({200, 300});
    Room room;
    assert(player.getLevel() == 1 && player.getExperience() == 0 && player.getAttributePoints() == 0);
    assert(!player.spendPoint(Attribute::Strength));
    player.gainExperience(320); // 50 + 100 + 150, excedente 20.
    assert(player.getLevel() == 4 && player.getExperience() == 20 && player.getAttributePoints() == 3);
    assert(player.spendPoint(Attribute::Strength));
    assert(player.spendPoint(Attribute::Agility));
    player.takeDamage(35);
    assert(player.spendPoint(Attribute::Vitality));
    assert(player.getMaxHealth() == 80 && player.getHealth() == 45 && player.getHealing() == 20);
    assert(std::abs(player.getSpeed() - 252) < 0.001f);
    assert(std::abs(player.getDamageMultiplier() - 1.1f) < 0.001f);
    player.boostDamage(0.25f, 10);
    assert(std::abs(player.getDamageMultiplier() - 1.375f) < 0.001f);
    player.update({}, 11, room);
    assert(std::abs(player.getDamageMultiplier() - 1.1f) < 0.001f);
    assert(std::abs(player.getSpeed() - 252) < 0.001f); // Recalculo nao acumula modificadores.
    assert(player.heal(100) == 35 && player.getHealing() == 55);
    assert(player.heal(30) == 0 && player.getHealing() == 55);

    Player moving({100, 600});
    moving.update({1, 0}, 0.5f, room);
    assert(std::abs(moving.getDistance() - 120) < 0.001);
    moving.enterRoom({100, 600}); // Teleporte nao conta.
    moving.update({1, 0}, 0.24f, room, true);
    assert(std::abs(moving.getDistance() - 312) < 0.001);
    moving.enterRoom({Config::wallThickness + Config::playerHalfSize, 600});
    moving.update({-1, 0}, 1, room);
    assert(std::abs(moving.getDistance() - 312) < 0.001); // Parede nao soma movimento ficticio.
    moving.recordDefeat(20);
    moving.recordCollection();
    assert(moving.getScore(2) == 547); // 100 + 50 + 400 - piso(312/100).
    moving.recordVictory();
    moving.recordVictory();
    assert(moving.getScore(2) == 1547);
    assert(player.getScore(0) == 0); // Penalidade nunca gera pontuacao negativa.

    Player fighter({200, 300});
    Combat combat;
    room.getEnemies().emplace_back(Vector2{240, 300});
    room.getEnemies()[0].takeDamage(40);
    combat.update(0, false, fighter, room);
    combat.update(0, false, fighter, room);
    assert(fighter.getDefeatedCount() == 1 && fighter.getExperience() == 20);
    fighter.gainExperience(30);
    fighter.spendPoint(Attribute::Strength);
    room.getEnemies().emplace_back(Vector2{240, 300});
    combat.update(0, true, fighter, room);
    assert(std::abs(room.getEnemies()[0].getHealth() - 18) < 0.001f);

    Game game;
    game.startNewGame();
    game.dungeon.currentRoom().getEnemies().emplace_back(Vector2{320, 300});
    game.player.gainExperience(40);
    game.dungeon.currentRoom().getEnemies()[0].takeDamage(40);
    game.update(0.01f);
    assert(game.state == GameState::Attributes && game.player.getAttributePoints() == 1);
    const float time = game.elapsedTime;
    const double distance = game.player.getDistance();
    game.input.movement = {1, 0};
    game.update(10);
    assert(game.elapsedTime == time && game.player.getDistance() == distance);
    game.input = {};
    game.selectedOption = 1;
    game.input.confirm = true;
    game.update(0);
    assert(game.player.getMaxHealth() == 80 && game.player.getAttributePoints() == 0);
    game.update(0); // Sem pontos, nao gasta novamente.
    assert(game.player.getMaxHealth() == 80);
    game.input = {};
    game.input.escape = true;
    game.update(0);
    assert(game.state == GameState::Playing);
    game.input = {};
    game.input.attributes = true;
    game.update(0);
    assert(game.state == GameState::Attributes);
    game.startNewGame();
    assert(game.player.getLevel() == 1 && game.player.getDefeatedCount() == 0);
    assert(game.player.getHealing() == 0 && game.player.getDistance() == 0);
    assert(game.player.getMaxHealth() == 60 && game.player.getScore(1) == 200);
    std::cout << "OK: XP, atributos, estatisticas, pontuacao, pausa e reinicio.\n";
}
