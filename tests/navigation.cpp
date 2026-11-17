#include "Game.h"
#include "raymath.h"
#include <cassert>
#include <chrono>
#include <iostream>

int main()
{
    Room room;
    const Vector2 start{350, 300}, end{550, 300};
    assert(!room.lineClear(start, end, 16));
    const auto path = room.findPath(start, end);
    assert(!path.empty());
    for (Vector2 point : path) assert(room.freePosition(point, 16));
    Player player(end);
    Enemy chaser(start);
    for (int i = 0; i < 1200; ++i)
    {
        chaser.update(0.01f, room, player);
        assert(room.freePosition(chaser.getPosition(), 16));
    }
    assert(Vector2Distance(chaser.getPosition(), end) < 40);
    Room sealed;
    sealed.addBox({300, 32, 48, 736});
    assert(sealed.findPath({200, 300}, {350, 300}).empty());
    sealed.damageBox(0, 40);
    assert(!sealed.findPath({200, 300}, {350, 300}).empty());

    Room open;
    Player target({300, 300});
    Enemy shooter({200, 300}, EnemyType::Shooter);
    shooter.update(0.01f, open, target);
    assert(open.getEnemyShots().size() == 1);
    assert(shooter.getPosition().x > 200); // Aproxima-se mesmo enquanto dispara.
    const Vector2 firstPosition = shooter.getPosition();
    shooter.update(0.1f, open, target);
    assert(shooter.getPosition().x > firstPosition.x);
    assert(open.getEnemyShots().size() == 1); // Movimento nao reinicia a cadencia.
    open.updateEnemyShots(0.4f, target);
    assert(target.getHealth() == 50 && open.getEnemyShots().empty());
    open.getEnemyShots().push_back({{350, 300}, {500, 0}});
    target.enterRoom({550, 300});
    open.updateEnemyShots(1, target);
    assert(open.getEnemyShots().empty() && target.getHealth() == 50); // Parede bloqueia disparo varrido.

    Player near({350, 600});
    Enemy patrol({200, 600}, EnemyType::Patrol), regular({200, 600});
    patrol.update(0.1f, open, near); regular.update(0.1f, open, near);
    assert(patrol.getPosition().x > regular.getPosition().x);
    Player distant({1100, 700});
    Enemy wander({150, 150});
    for (int i = 0; i < 200; ++i) wander.update(0.01f, open, distant);
    assert(Vector2Distance(wander.getPosition(), {150, 150}) > 1);
    assert(open.freePosition(wander.getPosition(), 16));
    const auto begin = std::chrono::steady_clock::now();
    for (int i = 0; i < 40; ++i) open.getEnemies().emplace_back(Vector2{150, 150}, EnemyType::Patrol);
    for (int frame = 0; frame < 240; ++frame)
        for (Enemy& enemy : open.getEnemies()) enemy.update(1.0f/240, open, distant);
    const auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now()-begin).count();
    assert(duration < 2000);
    std::cout << "OK: BFS, contorno, passagem reaberta, tiros, patrulha, vaguear e 40 inimigos (" << duration << "ms).\n";
}
