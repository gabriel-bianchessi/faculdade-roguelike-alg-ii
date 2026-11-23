#include "Game.h"
#include "raymath.h"
#include <cassert>
#include <iostream>
#include <set>

int main()
{
    std::set<int> types;
    std::set<int> layouts;
    for (unsigned int seed = 0; seed < 100; ++seed)
    {
        Dungeon dungeon(seed);
        assert(dungeon.findRoom(1)->getEnemies().empty() && dungeon.findRoom(8)->getEnemies().empty());
        dungeon.currentIndex=9;
        assert(dungeon.interactSecret(dungeon.getSecretPosition()));
        dungeon.currentIndex=0;
        for (int id = 1; id <= 13; ++id)
        {
            const Room& room = *dungeon.findRoom(id);
            layouts.insert(static_cast<int>(room.getBounds().width));
            std::vector<Vector2> occupied;
            for (const Enemy& enemy : room.getEnemies())
            {
                assert(room.freePosition(enemy.getPosition(), enemy.getType() == EnemyType::Boss ? 28 : 16));
                assert(Vector2Distance(enemy.getPosition(), room.getSpawn()) >= 180);
                types.insert(static_cast<int>(enemy.getType()));
                occupied.push_back(enemy.getPosition());
            }
            for (const Item& item : room.getItems())
            {
                assert(room.freePosition(item.position, 12));
                occupied.push_back(item.position);
            }
            for (std::size_t i = 0; i < occupied.size(); ++i)
            {
                for (std::size_t j = i+1; j < occupied.size(); ++j) assert(Vector2Distance(occupied[i], occupied[j]) >= 26);
                for (const Connection& door : room.getConnections())
                    assert(Vector2Distance(occupied[i], {door.door.x+door.door.width/2, door.door.y+door.door.height/2}) >= 60);
            }
            for (const Connection& door : room.getConnections())
            {
                const Vector2 center{door.door.x+door.door.width/2,door.door.y+door.door.height/2};
                assert(!room.findPath(room.getSpawn(), center).empty());
                const Room& destination = *dungeon.findRoom(door.to);
                assert(destination.freePosition(door.arrival, 16));
                assert(!destination.findPath(destination.getSpawn(), door.arrival).empty());
            }
            if (room.getWaveCount() > 0)
            {
                Room simulated = room;
                Player visitor(simulated.getSpawn());
                while (simulated.hasPendingWaves())
                {
                    simulated.getEnemies().clear();
                    const int previous = simulated.getWaveNumber();
                    simulated.updateWaves(Config::waveDelay+0.01f,visitor);
                    assert(simulated.getWaveNumber() == previous+1);
                    for (const Enemy& enemy : simulated.getEnemies())
                        assert(Vector2Distance(enemy.getPosition(),visitor.getPosition()) >= Config::waveSpawnDistance);
                }
            }
        }
    }
    assert(types.size() == 4 && layouts.size() >= 6);
    Dungeon a(100), b(100), c(200);
    const Vector2 pa = a.findRoom(5)->getEnemies()[0].getPosition();
    const Vector2 pb = b.findRoom(5)->getEnemies()[0].getPosition();
    const Vector2 pc = c.findRoom(5)->getEnemies()[0].getPosition();
    assert(Vector2Distance(pa,pb) == 0 && Vector2Distance(pa,pc) > 0);
    std::cout << "OK: 100 sementes, 13 salas incluindo tesouro, rotas, surgimentos, entradas e tipos de inimigo.\n";
}
