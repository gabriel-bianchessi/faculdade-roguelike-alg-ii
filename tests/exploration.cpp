#include "Dungeon.h"
#include "Combat.h"
#include "Player.h"

#include <cassert>
#include <iostream>
#include <queue>

Vector2 center(Rectangle bounds)
{
    return {bounds.x + bounds.width / 2, bounds.y + bounds.height / 2};
}

Connection connectionTo(const Room& room, int id)
{
    for (const Connection& connection : room.getConnections())
        if (connection.to == id) return connection;
    assert(false);
    return {};
}

bool travel(Dungeon& dungeon, Player& player, int id)
{
    player.enterRoom(dungeon.currentRoom().getSpawn());
    dungeon.update(0.31f, player); // Sai do gatilho anterior e aguarda a recarga.
    const Connection connection = connectionTo(dungeon.currentRoom(), id);
    player.enterRoom(center(connection.door));
    return dungeon.update(0.01f, player);
}

void freePosition(const Room& room, Vector2 position)
{
    const float half = Config::playerHalfSize;
    const Rectangle bounds{position.x - half, position.y - half, half * 2, half * 2};
    for (const Rectangle& solid : room.getSolids()) assert(!CheckCollisionRecs(bounds, solid));
}

void accessible(const Room& room, Vector2 start, Vector2 end)
{
    const int columns = room.getColumns();
    const int rows = room.getRows();
    const int startX = static_cast<int>(start.x / Config::cellSize);
    const int startY = static_cast<int>(start.y / Config::cellSize);
    const int endX = static_cast<int>(end.x / Config::cellSize);
    const int endY = static_cast<int>(end.y / Config::cellSize);
    assert(room.isWalkable(startX, startY));
    assert(room.isWalkable(endX, endY));
    std::vector<bool> visited(columns * rows);
    std::queue<int> pending;
    pending.push(startY * columns + startX);
    visited[pending.front()] = true;
    while (!pending.empty())
    {
        const int cell = pending.front();
        pending.pop();
        const int x = cell % columns, y = cell / columns;
        const int dx[] = {-1, 1, 0, 0}, dy[] = {0, 0, -1, 1};
        for (int axis = 0; axis < 4; ++axis)
        {
            const int nextX = x + dx[axis], nextY = y + dy[axis];
            if (!room.isWalkable(nextX, nextY)) continue;
            const int next = nextY * columns + nextX;
            if (visited[next]) continue;
            visited[next] = true;
            pending.push(next);
        }
    }
    assert(visited[endY * columns + endX]);
}

int main()
{
    Dungeon dungeon;
    Player player(dungeon.currentRoom().getSpawn());
    assert(dungeon.getDiscoveredCount() == 1);
    assert(!dungeon.hasKey());
    assert(dungeon.findRoom(999) == nullptr);
    // Todas as chegadas e portas sao livres e acessiveis pela grade de navegacao.
    for (int id : {1, 2, 3, 4, 5, 6, 7, 9, 12})
    {
        Room& room = *dungeon.findRoom(id);
        freePosition(room, room.getSpawn());
        for (const Connection& connection : room.getConnections())
        {
            assert(connection.from == id);
            freePosition(room, center(connection.door));
            accessible(room, room.getSpawn(), center(connection.door));
            Room& destination = *dungeon.findRoom(connection.to);
            freePosition(destination, connection.arrival);
            accessible(destination, destination.getSpawn(), connection.arrival);
        }
        assert(!room.isWalkable(-1, 0));
        assert(!room.isWalkable(0, 0));
    }
    accessible(*dungeon.findRoom(9), dungeon.findRoom(9)->getSpawn(), dungeon.findRoom(9)->getKeyPosition());

    Room& first = dungeon.currentRoom();
    first.getEnemies()[0].takeDamage(100);
    Combat combat;
    combat.update(0, false, player, first);
    assert(first.getEnemies().empty());
    const Vector2 remembered{800, 740};
    first.reveal(remembered);
    const int column = static_cast<int>(remembered.x / Config::cellSize);
    const int row = static_cast<int>(remembered.y / Config::cellSize);
    assert(first.getCell(column, row).visible);
    first.reveal(first.getSpawn());
    assert(first.getCell(column, row).explored);
    assert(!first.getCell(column, row).visible);
    assert(travel(dungeon, player, 2));
    assert(dungeon.currentRoom().getId() == 2);
    assert(!dungeon.update(0, player)); // Nao volta automaticamente.
    dungeon.currentRoom().getEnemies()[0].takeDamage(8);
    assert(travel(dungeon, player, 1));
    assert(first.getEnemies().empty());
    assert(first.getCell(column, row).explored);
    assert(dungeon.getDiscoveredCount() == 2);
    assert(travel(dungeon, player, 2));
    assert(dungeon.currentRoom().getEnemies()[0].getHealth() == 32);
    for (int id : {3, 4, 5, 6, 7}) assert(travel(dungeon, player, id));
    assert(dungeon.nearLockedDoor(center(connectionTo(dungeon.currentRoom(), 12).door)));
    assert(!travel(dungeon, player, 12));
    assert(dungeon.currentRoom().getId() == 7);
    assert(dungeon.getDiscoveredCount() == 7);
    for (int id : {6, 5, 4, 3, 9}) assert(travel(dungeon, player, id));
    player.enterRoom(dungeon.currentRoom().getKeyPosition());
    dungeon.update(0, player);
    assert(dungeon.hasKey());
    assert(!dungeon.currentRoom().hasKey());
    for (int id : {3, 9}) assert(travel(dungeon, player, id));
    assert(!dungeon.currentRoom().hasKey());
    for (int id : {3, 4, 5, 6, 7, 12}) assert(travel(dungeon, player, id));
    assert(dungeon.currentRoom().getId() == 12);
    assert(dungeon.getDiscoveredCount() == 9);
    assert(!dungeon.nearLockedDoor(player.getPosition()));
    assert(travel(dungeon, player, 7));
    assert(dungeon.hasKey());
    dungeon = Dungeon();
    assert(!dungeon.hasKey());
    assert(dungeon.getDiscoveredCount() == 1);
    assert(dungeon.findRoom(9)->hasKey());
    assert(!dungeon.findRoom(9)->isDiscovered());
    std::cout << "OK: conexoes, chegadas, grade, memoria, persistencia, chave, porta e reinicio.\n";
}
