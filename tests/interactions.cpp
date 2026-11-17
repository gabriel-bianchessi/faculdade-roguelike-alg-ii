#include "Game.h"
#include "raymath.h"
#include <cassert>
#include <iostream>

int main()
{
    Room room;
    Player player({200, 300});
    room.addBox({240, 280, 48, 48});
    assert(!room.isWalkable(8, 9));
    assert(room.move(player.getPosition(), {100, 0}, 14).x == 226);
    Combat bladeCombat;
    bladeCombat.update(0, true, player, room);
    assert(room.getBoxes()[0].health == 20);
    bladeCombat.update(0.05f, false, player, room);
    assert(room.getBoxes()[0].health == 20);
    bladeCombat.update(0.35f, true, player, room);
    assert(room.getBoxes()[0].health == 0 && room.isWalkable(8, 9));
    assert(room.move(player.getPosition(), {100, 0}, 14).x == 300);

    room.addBox({240, 280, 48, 48});
    Combat gunCombat;
    gunCombat.switchWeapon();
    gunCombat.update(0.1f, true, player, room);
    assert(room.getBoxes()[1].health == 32 && gunCombat.getProjectiles().empty());
    room.getEnemies().emplace_back(Vector2{220, 300});
    player.enterRoom({350, 300});
    for (int i = 0; i < 1000; ++i) room.getEnemies()[0].update(0.01f, room, player);
    assert(room.getBoxes()[1].health < 32); // Inimigo ataca a caixa ao alcance.
    for (int i = 0; i < 4; ++i)
    {
        room.getEnemyShots().push_back({{200, 300}, {300, 0}});
        room.updateEnemyShots(0.3f, player);
    }
    assert(room.getBoxes()[1].health == 0); // Disparos inimigos tambem destroem caixas.

    Room trapped;
    Player victim({200, 300});
    trapped.getSpikes().push_back({{180, 280, 80, 80}, 7});
    trapped.getEnemies().emplace_back(Vector2{220, 300});
    trapped.updateHazards(2, victim);
    assert(trapped.getSpikes()[0].state == SpikeState::Warning && victim.getHealth() == 60);
    trapped.updateHazards(1.01f, victim);
    assert(trapped.getSpikes()[0].state == SpikeState::Active && victim.getHealth() == 45);
    assert(trapped.getEnemies()[0].getHealth() == 25);
    trapped.updateHazards(0.01f, victim);
    assert(trapped.getEnemies()[0].getHealth() == 25);
    trapped.getLevers().push_back({{200, 300}, 7});
    assert(!trapped.interactLever({500, 300}));
    assert(trapped.interactLever(victim.getPosition()));
    trapped.updateHazards(10, victim);
    assert(victim.getHealth() == 45 && trapped.getSpikes()[0].disabled);

    Game game;
    game.startNewGame();
    game.player.enterRoom(game.dungeon.currentRoom().getNpc().position);
    game.input.interact = true;
    game.update(0.1f);
    assert(game.state == GameState::Dialogue && game.dialogueLine == 0);
    game.input = {};
    const float time = game.elapsedTime;
    game.update(10);
    assert(game.elapsedTime == time && game.dialogueLine == 0);
    for (int i = 0; i < 3; ++i) { game.input.interact = true; game.update(1); }
    assert(game.state == GameState::Playing && game.dungeon.currentRoom().getNpc().talked);
    assert(game.dungeon.findRoom(8)->getEnemies().empty());
    // Encerrar dialogo do curandeiro na propria sala, com persistencia no retorno.
    game.dungeon.currentIndex = 7; // Sala 8 na ordem de criacao.
    assert(game.dungeon.currentRoom().getId() == 8);
    game.player.enterRoom(game.dungeon.currentRoom().getNpc().position);
    game.player.takeDamage(40);
    game.input.interact = true;
    game.update(0);
    for (int i = 0; i < 3; ++i) game.update(0);
    assert(game.player.getHealth() == 60 && game.player.getHealing() == 40);
    assert(game.dungeon.isShortcutOpen() && game.dungeon.currentRoom().getNpc().healingUsed);
    assert(game.dungeon.currentRoom().getConnections().size() == 2);
    game.player.update({}, 1, game.dungeon.currentRoom());
    game.player.takeDamage(20);
    game.update(0);
    for (int i = 0; i < 3; ++i) game.update(0);
    assert(game.player.getHealth() == 40 && game.player.getHealing() == 40);
    assert(game.dungeon.currentRoom().getConnections().size() == 2);
    game.startNewGame();
    assert(!game.dungeon.isShortcutOpen() && !game.dungeon.findRoom(8)->getNpc().healingUsed);
    Dungeon persistent(1);
    persistent.currentIndex = 3;
    Player visitor(persistent.currentRoom().getSpawn());
    Room& altered = persistent.currentRoom();
    altered.damageBox(0, 40);
    const Vector2 lever = altered.getLevers()[0].position;
    assert(altered.interactLever(lever));
    auto travel = [&](int targetRoom)
    {
        persistent.currentRoom().getEnemies().clear(); // Isola a persistencia dos mecanismos.
        visitor.enterRoom(persistent.currentRoom().getSpawn());
        persistent.update(0.31f, visitor);
        for (const Connection& door : persistent.currentRoom().getConnections())
            if (door.to == targetRoom)
            {
                visitor.enterRoom({door.door.x+door.door.width/2,door.door.y+door.door.height/2});
                assert(persistent.update(0.01f, visitor));
                return;
            }
        assert(false);
    };
    travel(5); travel(4);
    assert(altered.getBoxes()[0].health == 0 && altered.getLevers()[0].activated && altered.getSpikes()[0].disabled);
    persistent = Dungeon(1);
    assert(persistent.findRoom(4)->getBoxes()[0].health == 40 && !persistent.findRoom(4)->getSpikes()[0].disabled);
    std::cout << "OK: caixas, grade, ataques, espinhos, alavancas, dialogos e cura unica.\n";
}
