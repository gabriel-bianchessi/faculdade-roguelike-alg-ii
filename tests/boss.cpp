#include "Game.h"
#include "raymath.h"
#include <cassert>
#include <iostream>

int main()
{
    Room arena(1664,1088,12);
    arena.setObstacles({});
    Player target({300,500});
    Enemy boss({800,500},EnemyType::Boss);
    assert(boss.getHealth() == 400 && boss.getExperienceReward() == 100);
    boss.update(1.51f, arena, target);
    assert(boss.getBossState() == BossState::ChargeWarning);
    const Vector2 warningPosition = boss.getPosition();
    boss.update(0.5f, arena, target);
    assert(boss.getBossState() == BossState::ChargeWarning);
    assert(Vector2Distance(warningPosition,boss.getPosition()) == 0);
    boss.update(0.41f, arena, target);
    assert(boss.getBossState() == BossState::Charge);
    boss.update(0.46f, arena, target);
    assert(boss.getBossState() == BossState::Recovery);
    assert(arena.freePosition(boss.getPosition(),28));
    boss.update(0.81f, arena, target);
    boss.update(1.51f, arena, target);
    assert(boss.getBossState() == BossState::FanWarning);
    boss.update(0.91f, arena, target);
    assert(arena.getEnemyShots().size() == 7);
    assert(arena.getEnemyShots().front().velocity.y != arena.getEnemyShots().back().velocity.y);
    boss.takeDamage(201);
    boss.update(0.81f, arena, target);
    assert(boss.getBossState() == BossState::Hunt && boss.stateTimer == 0.9f);
    boss.update(0.91f, arena, target);
    assert(boss.getBossState() == BossState::ChargeWarning && boss.stateTimer == 0.65f);
    boss.update(0.66f, arena, target);
    boss.update(3, arena, target); // Varredura mantem chefe dentro da sala mesmo num quadro enorme.
    assert(arena.freePosition(boss.getPosition(),28));

    Game game;
    game.startNewGame();
    game.dungeon.currentIndex = 11;
    assert(game.dungeon.currentRoom().getId() == 12);
    game.player.enterRoom({200,500});
    game.dungeon.currentRoom().getEnemies()[0].takeDamage(400);
    game.update(0.01f);
    assert(game.state == GameState::Victory && game.player.hasWon());
    assert(game.player.getDefeatedCount() == 1 && game.player.getLevel() == 2 && game.player.getExperience() == 50);
    const int score = game.player.getScore(game.dungeon.getDiscoveredCount());
    game.update(10);
    assert(score == game.player.getScore(game.dungeon.getDiscoveredCount()));
    game.input.confirm = true; game.update(0);
    assert(game.state == GameState::Playing && !game.player.hasWon());
    assert(game.player.getDefeatedCount() == 0 && game.player.getLevel() == 1);
    assert(game.dungeon.findRoom(12)->getEnemies()[0].getHealth() == 400);
    std::cout << "OK: chefe, avisos, investida, leque, furia, XP, vitoria unica e reinicio.\n";
}
