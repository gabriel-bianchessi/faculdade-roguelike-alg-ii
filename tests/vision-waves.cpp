#include "Game.h"
#include "raymath.h"
#include <cassert>
#include <iostream>

void vision()
{
    Room room;
    room.setObstacles({{480,160,48,360}});
    room.reveal({300,300},{1,0});
    assert(room.isVisible({430,300}));
    assert(room.isVisible({480,300})); // A face da parede pode ser vista.
    assert(!room.isVisible({580,300})); // O chao atras da parede fica oculto.
    assert(!room.isVisible({100,300})); // Fora do campo direcional.
    assert(room.isVisible({220,300})); // Percepcao proxima ao redor do personagem.
    assert(room.isVisible({520,80})); // Mais longe que o antigo raio de 240.
    room.reveal({300,300},{-1,0});
    assert(room.isVisible({100,300}));
    assert(!room.isVisible({520,80}));
    assert(room.getCell(16,2).explored); // Memoria ao virar a mira.
    room.reveal({300,300},{1,0});
    room.setObstacles({}); // Invalida a visao mesmo sem mudar posicao/mira.
    room.reveal({300,300},{1,0});
    assert(room.isVisible({580,300}));
    room.addBox({480,280,48,64});
    room.reveal({300,300},{1,0});
    assert(!room.isVisible({580,300}));
    room.damageBox(0,40);
    room.reveal({300,300},{1,0});
    assert(room.isVisible({580,300}));
    const Camera2D camera = room.getCamera(960,640,{600,400});
    assert(camera.zoom == 1.0f);
    assert(Config::exploredShadow < 180 && Config::unexploredShadow < 255);
}

void waves()
{
    Room room;
    room.setObstacles({});
    Player player({200,300});
    room.getEnemies().emplace_back(Vector2{600,300});
    std::vector<std::vector<Enemy>> queued{{Enemy({200,300}),Enemy({700,300},EnemyType::Shooter)},
        {Enemy({600,500},EnemyType::Patrol)}};
    room.configureWaves(queued,{{200,300},{700,300},{600,500},{900,600}});
    assert(room.getWaveNumber() == 1 && room.getWaveCount() == 3);
    room.updateWaves(20,player);
    assert(room.getWaveNumber() == 1 && room.getEnemies().size() == 1);
    room.getEnemies().clear();
    room.updateWaves(2,player);
    assert(room.isWaveWaiting() && room.getEnemies().empty());
    room.updateWaves(1.01f,player);
    assert(room.getWaveNumber() == 2 && room.getEnemies().size() == 2);
    for (const Enemy& enemy : room.getEnemies())
    {
        assert(Vector2Distance(enemy.getPosition(),player.getPosition()) >= Config::waveSpawnDistance);
        assert(room.freePosition(enemy.getPosition(),20));
    }
    assert(Vector2Distance(room.getEnemies()[0].getPosition(),room.getEnemies()[1].getPosition()) >= 64);
    room.getEnemies().clear(); room.updateWaves(3.01f,player);
    assert(room.getWaveNumber() == 3 && room.getEnemies().size() == 1 && !room.hasPendingWaves());
    room.getEnemies().clear(); room.updateWaves(50,player);
    assert(room.getEnemies().empty()); // Onda final nao se repete.

    Game game;
    game.startNewGame();
    game.dungeon.currentIndex = 1;
    Room& combatRoom = game.dungeon.currentRoom();
    game.player.enterRoom(combatRoom.getSpawn());
    combatRoom.getEnemies().clear();
    game.update(1);
    const float remaining = combatRoom.getWaveRemaining();
    game.input.escape = true; game.update(0);
    game.input = {}; game.update(10);
    assert(game.state == GameState::Paused && combatRoom.getWaveRemaining() == remaining);
    game.input.escape = true; game.update(0); game.input = {};
    // O temporizador nao reinicia na saida, nem avanca fora da sala ativa.
    auto travel = [&](int target)
    {
        game.player.enterRoom(game.dungeon.currentRoom().getSpawn());
        game.dungeon.update(0.31f,game.player);
        for (const Connection& door : game.dungeon.currentRoom().getConnections())
            if (door.to == target)
            {
                game.player.enterRoom({door.door.x+door.door.width/2,door.door.y+door.door.height/2});
                assert(game.dungeon.update(0.01f,game.player)); return;
            }
        assert(false);
    };
    travel(1); game.update(10); travel(2);
    assert(combatRoom.getWaveRemaining() == remaining);
    game.startNewGame();
    assert(game.dungeon.findRoom(2)->getWaveNumber() == 1);
    for (int id : {1,8,12}) assert(game.dungeon.findRoom(id)->getWaveCount() == 0);
    for (int id : {2,3,4,5,6,7,9,10,11}) assert(game.dungeon.findRoom(id)->getWaveCount() >= 2);
}

int main()
{
    vision(); waves();
    std::cout << "OK: visao direcional, oclusao, memoria, zoom, ondas, distancia segura, pausa e persistencia.\n";
}
