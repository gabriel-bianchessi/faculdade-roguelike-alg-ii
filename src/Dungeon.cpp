#include "Dungeon.h"

#include "Player.h"
#include "Scenery.h"
#include "raymath.h"

#include <algorithm>

Dungeon::Dungeon(unsigned int seed)
{
    const Vector2 dimensions[] = {{1120,768}, {1280,800}, {1440,864}, {1280,960},
        {1536,896}, {1408,960}, {1600,1024}, {960,640}, {1152,768}, {1280,864}, {1376,800}, {1664,1088}};
    for (int id = 1; id <= 12; ++id)
    {
        rooms.emplace_back(dimensions[id-1].x, dimensions[id-1].y, id);
        Room& room = rooms.back();
        const float w = dimensions[id-1].x, h = dimensions[id-1].y;
        if (id == 1) room.setObstacles({{w*0.6f,h*0.3f,64,160}, {w*0.3f,h*0.7f,180,48}});
        if (id == 8) room.setObstacles({});
        if (id == 9 || id == 10) room.setObstacles({{w*0.38f,h*0.28f,48,h*0.40f}, {w*0.65f,h*0.22f,80,80}});
        if (id == 11) room.setObstacles({{w*0.4f,h*0.3f,160,48}, {w*0.6f,h*0.65f,160,48}});
        if (id == 12) room.setObstacles({{w*0.25f,h*0.22f,64,64}, {w*0.72f,h*0.22f,64,64},
            {w*0.25f,h*0.72f,64,64}, {w*0.72f,h*0.72f,64,64}});
    }
    for (int id = 1; id < 7; ++id) connect(id, id + 1);
    connect(7, 12, false, true);
    connect(2, 8, true);
    connect(3, 9, true);
    findRoom(1)->getNpc() = {NpcType::Watchman, {250, 220}};
    findRoom(8)->getNpc() = {NpcType::Healer, {250, 220}};
    for (int id : {4, 5, 6, 7})
    {
        Room& room = *findRoom(id);
        room.addBox({600, 300, 48, 48});
        room.addBox({648, 300, 48, 48});
        const Rectangle bounds = room.getBounds();
        room.getSpikes().push_back({{bounds.width*0.4f, bounds.height*0.78f, 180, 48}, id});
        room.getLevers().push_back({{bounds.width*0.4f-70, bounds.height*0.78f+24}, id});
    }
    connect(5, 10, true);
    connect(6, 11, true);
    findRoom(1)->getItems().push_back({ItemType::Shield, {230, 160}, WeaponType::Sword, Config::shieldAmount});
    findRoom(2)->getItems().push_back({ItemType::HealthKit, {250, 160}});
    findRoom(3)->getItems().push_back({ItemType::DamageBoost, {250, 160}, WeaponType::Sword, Config::damageBonusPercent});
    findRoom(10)->getItems().push_back({ItemType::Weapon, {250, 160}, WeaponType::Spear});
    findRoom(11)->getItems().push_back({ItemType::Weapon, {250, 160}, WeaponType::Shotgun});
    findRoom(10)->getItems().push_back({ItemType::HealthKit, {250, 240}});
    findRoom(11)->getItems().push_back({ItemType::DamageBoost, {250, 240}, WeaponType::Sword, Config::damageBonusPercent});
    findRoom(9)->placeKey({250, 160});
    findRoom(12)->getEnemies().emplace_back(Vector2{dimensions[11].x*0.58f, dimensions[11].y*0.48f}, EnemyType::Boss);
    populate(seed);
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
    room.reveal(player.getPosition(),player.getDirection());
    if (room.collectKey(player.getPosition()))
    {
        keyCollected = true;
        player.recordCollection();
    }
    const Vector2 position = player.getPosition();
    const float half = Config::playerHalfSize;
    const Rectangle playerBounds{position.x - half, position.y - half, half * 2, half * 2};
    bool touchingDoor = false;
    for (const Connection& connection : room.getConnections())
    {
        if (!CheckCollisionRecs(playerBounds, connection.door)) continue;
        touchingDoor = true;
        if (!doorArmed || transitionRemaining > 0 || room.hasLivingEnemies() ||
            (connection.locked && !keyCollected)) continue;
        Room* destination = findRoom(connection.to);
        // As conexoes sao criadas apenas entre salas existentes e chegadas livres.
        room.getEnemyShots().clear();
        currentIndex = static_cast<std::size_t>(destination - rooms.data());
        player.enterRoom(connection.arrival);
        discoveredCount += destination->discover();
        destination->reveal(player.getPosition(),player.getDirection());
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
        const bool combatBlocked = currentRoom().hasLivingEnemies();
        const Color color = combatBlocked || (connection.locked && !keyCollected) ? RED : Config::accent;
        const Rectangle door = connection.door;
        const Rectangle bounds = currentRoom().getBounds();
        const Scenery::Piece portal = door.width > door.height
            ? (door.y < bounds.height/2 ? Scenery::PortalTop : Scenery::PortalBottom)
            : (door.x < bounds.width/2 ? Scenery::PortalLeft : Scenery::PortalRight);
        if (!Scenery::draw(portal,door)) DrawRectangleRec(door,Config::panel);
        DrawRectangleLinesEx(connection.door, 3, color);
        DrawRectangle(connection.door.x+5,connection.door.y+9,28,26,Fade(Config::background,0.8f));
        DrawText(TextFormat("%i", connection.to), static_cast<int>(connection.door.x + 8),
                 static_cast<int>(connection.door.y + 12), 20, color);
        if (combatBlocked)
        {
            DrawLineEx({door.x+5,door.y+door.height-10},{door.x+door.width-5,door.y+door.height-10},4,color);
        }
    }
    if (currentRoom().hasKey())
    {
        const Vector2 position = currentRoom().getKeyPosition();
        DrawCircleLines(static_cast<int>(position.x), static_cast<int>(position.y), 10, GOLD);
        DrawLineEx({position.x + 8, position.y}, {position.x + 23, position.y}, 4, GOLD);
        DrawLineEx({position.x + 20, position.y}, {position.x + 20, position.y + 8}, 4, GOLD);
    }
}

void Dungeon::finishDialogue(Player& player)
{
    Npc& npc = currentRoom().getNpc();
    npc.talked = true;
    if (npc.type != NpcType::Healer) return;
    if (!npc.healingUsed)
    {
        player.heal(player.getMaxHealth());
        npc.healingUsed = true;
        npc.healingAnimationRemaining = 1.28f;
    }
    if (shortcutOpen) return;
    shortcutOpen = true;
    // Atalho em portas proprias, sem sobrepor as conexoes ja existentes.
    Room& safe = *findRoom(8);
    Room& main = *findRoom(4);
    safe.getConnections().push_back({8, 4, {safe.getBounds().width-80, 180, 48, 64}, {main.getBounds().width-132, 212}});
    main.getConnections().push_back({4, 8, {main.getBounds().width-80, 180, 48, 64}, {safe.getBounds().width-132, 212}});
}

void Dungeon::populate(unsigned int seed)
{
    std::mt19937 random(seed);
    for (Room& room : rooms)
    {
        const int id = room.getId();
        if (id == 1 || id == 8 || id == 12) continue;
        const Rectangle bounds = room.getBounds();
        // Posicoes candidatas manuais, escaladas conforme cada sala.
        std::vector<Vector2> candidates;
        for (Vector2 fraction : {Vector2{0.20f,0.38f}, {0.24f,0.65f}, {0.45f,0.18f},
            {0.50f,0.44f}, {0.62f,0.40f}, {0.82f,0.60f}, {0.78f,0.82f}, {0.48f,0.82f}})
            candidates.push_back({bounds.width*fraction.x, bounds.height*fraction.y});
        std::shuffle(candidates.begin(), candidates.end(), random);
        std::vector<Vector2> occupied;
        for (const Item& item : room.getItems()) occupied.push_back(item.position);
        if (room.hasKey()) occupied.push_back(room.getKeyPosition());
        std::vector<Vector2> wavePositions;
        for (Vector2 position : candidates)
        {
            if (!room.freePosition(position,24)) continue;
            bool blocked = false;
            for (Vector2 other : occupied) if (Vector2Distance(position,other) < 80) blocked = true;
            for (const Connection& door : room.getConnections())
                if (Vector2Distance(position,{door.door.x+door.door.width/2,door.door.y+door.door.height/2}) < 120) blocked = true;
            if (!blocked) wavePositions.push_back(position);
        }
        int spawned = 0;
        const int desired = id < 4 ? 2 : 3 + static_cast<int>(random()%2);
        for (Vector2 position : candidates)
        {
            if (!room.freePosition(position, 24)) continue;
            bool blocked = Vector2Distance(position, room.getSpawn()) < 180;
            for (Vector2 other : occupied) if (Vector2Distance(position, other) < 80) blocked = true;
            for (const Connection& door : room.getConnections())
                if (Vector2Distance(position, {door.door.x+door.door.width/2,door.door.y+door.door.height/2}) < 120) blocked = true;
            if (blocked) continue;
            if (spawned < desired)
            {
                const EnemyType type = id == 2 ? EnemyType::Chaser :
                    static_cast<EnemyType>(random()%3);
                room.getEnemies().emplace_back(position, type);
                ++spawned;
            }
            else if (random()%2 == 0)
            {
                const ItemType type = static_cast<ItemType>(random()%3);
                const int amount = type == ItemType::DamageBoost ? Config::damageBonusPercent : Config::kitHeal;
                room.getItems().push_back({type, position, WeaponType::Sword, amount});
            }
            occupied.push_back(position);
        }
        std::vector<std::vector<Enemy>> reinforcements;
        const int extraWaves = id < 4 ? 1 : 2;
        for (int wave = 0; wave < extraWaves; ++wave)
        {
            std::shuffle(wavePositions.begin(),wavePositions.end(),random);
            std::vector<Enemy> group;
            const int count = std::min(static_cast<int>(wavePositions.size()),desired+wave+1);
            for (int i = 0; i < count; ++i)
                group.emplace_back(wavePositions[i], id == 2 ? EnemyType::Chaser : static_cast<EnemyType>(random()%3));
            if (!group.empty()) reinforcements.push_back(std::move(group));
        }
        room.configureWaves(std::move(reinforcements),std::move(wavePositions));
    }
}
