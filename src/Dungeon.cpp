#include "Dungeon.h"

#include "Player.h"
#include "raymath.h"

#include <algorithm>

Dungeon::Dungeon()
{
    // Salas provisorias: caminho principal e ramificacao obrigatoria da chave.
    for (int id : {1, 2, 3, 4, 5, 6, 7, 9, 12})
    {
        rooms.emplace_back(Config::roomWidth, Config::roomHeight, id);
        rooms.back().getEnemies().emplace_back(Vector2{320, 300});
    }
    for (int id = 1; id < 7; ++id) connect(id, id + 1);
    connect(7, 12, false, true);
    connect(3, 9, true);
    findRoom(9)->placeKey({250, 160});
    discoveredCount += currentRoom().discover();
    currentRoom().reveal(currentRoom().getSpawn());
}

Room* Dungeon::findRoom(int id)
{
    const auto found = std::find_if(rooms.begin(), rooms.end(),
        [id](const Room& room) { return room.getId() == id; });
    return found == rooms.end() ? nullptr : &*found;
}

void Dungeon::connect(int from, int to, bool vertical, bool locked)
{
    Room& source = *findRoom(from);
    Room& destination = *findRoom(to);
    const Rectangle a = source.getBounds();
    const Rectangle b = destination.getBounds();
    const float wall = Config::wallThickness;
    Rectangle exit;
    Rectangle entrance;
    Vector2 arrival;
    Vector2 returnPosition;
    if (vertical)
    {
        exit = {a.width * 0.2f - 32, a.height - wall - 48, 64, 48};
        entrance = {b.width * 0.2f - 32, wall, 64, 48};
        arrival = {b.width * 0.2f, wall + 100};
        returnPosition = {a.width * 0.2f, a.height - wall - 100};
    }
    else
    {
        exit = {a.width - wall - 48, a.height / 2 - 32, 48, 64};
        entrance = {wall, b.height / 2 - 32, 48, 64};
        arrival = {wall + 100, b.height / 2};
        returnPosition = {a.width - wall - 100, a.height / 2};
    }
    source.getConnections().push_back({from, to, exit, arrival, locked});
    destination.getConnections().push_back({to, from, entrance, returnPosition, locked});
}

bool Dungeon::update(float deltaTime, Player& player)
{
    transitionRemaining = std::max(0.0f, transitionRemaining - deltaTime);
    Room& room = currentRoom();
    room.reveal(player.getPosition());
    if (room.collectKey(player.getPosition())) keyCollected = true;
    const Vector2 position = player.getPosition();
    const float half = Config::playerHalfSize;
    const Rectangle playerBounds{position.x - half, position.y - half, half * 2, half * 2};
    bool touchingDoor = false;
    for (const Connection& connection : room.getConnections())
    {
        if (!CheckCollisionRecs(playerBounds, connection.door)) continue;
        touchingDoor = true;
        if (!doorArmed || transitionRemaining > 0 || (connection.locked && !keyCollected)) continue;
        Room* destination = findRoom(connection.to);
        // As conexoes sao criadas apenas entre salas existentes e chegadas livres.
        currentIndex = static_cast<std::size_t>(destination - rooms.data());
        player.enterRoom(connection.arrival);
        discoveredCount += destination->discover();
        destination->reveal(player.getPosition());
        transitionRemaining = Config::transitionCooldown;
        doorArmed = false;
        return true;
    }
    if (!touchingDoor) doorArmed = true;
    return false;
}

bool Dungeon::nearLockedDoor(Vector2 position) const
{
    if (keyCollected) return false;
    for (const Connection& connection : currentRoom().getConnections())
    {
        const Rectangle door = connection.door;
        if (connection.locked && Vector2Distance(position,
            {door.x + door.width / 2, door.y + door.height / 2}) < 100) return true;
    }
    return false;
}

void Dungeon::drawDetails() const
{
    for (const Connection& connection : currentRoom().getConnections())
    {
        const Color color = connection.locked && !keyCollected ? RED : Config::accent;
        DrawRectangleRec(connection.door, Config::panel);
        DrawRectangleLinesEx(connection.door, 3, color);
        DrawText(TextFormat("%i", connection.to), static_cast<int>(connection.door.x + 8),
                 static_cast<int>(connection.door.y + 12), 20, color);
    }
    if (currentRoom().hasKey())
    {
        const Vector2 position = currentRoom().getKeyPosition();
        DrawCircleLines(static_cast<int>(position.x), static_cast<int>(position.y), 10, GOLD);
        DrawLineEx({position.x + 8, position.y}, {position.x + 23, position.y}, 4, GOLD);
        DrawLineEx({position.x + 20, position.y}, {position.x + 20, position.y + 8}, 4, GOLD);
    }
}
