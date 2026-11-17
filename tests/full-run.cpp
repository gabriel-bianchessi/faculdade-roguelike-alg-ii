// Partida automatizada pelas mesmas entradas e atualizacoes usadas no jogo.
// Sem teleporte, dano artificial, cura extra ou alteracao dos atributos.
#include "Game.h"
#include "raymath.h"
#include <cassert>
#include <cmath>
#include <iostream>
#include <queue>

Vector2 navigate(const Room& room, Vector2 start, Vector2 goal)
{
    if (room.lineClear(start, goal, Config::playerHalfSize+1)) return Vector2Subtract(goal,start);
    const auto path = room.findPath(start,goal);
    Vector2 next = start;
    for (Vector2 point : path)
    {
        if (!room.lineClear(start,point,Config::playerHalfSize+0.5f)) break;
        next = point;
    }
    if (Vector2Distance(next,start) < 1 && !path.empty()) next = path[0];
    return Vector2Subtract(next,start);
}

Vector2 evade(const Room& room, const Player& player, const Enemy& target)
{
    const Vector2 start = player.getPosition();
    const Vector2 offset = Vector2Subtract(start,target.getPosition());
    const Vector2 tangent{-offset.y,offset.x};
    const float desired = target.getType() == EnemyType::Boss ? 310 : 220;
    float bestScore = -1e20f;
    Vector2 best{};
    for (int i = 0; i < 32; ++i)
    {
        const Vector2 move{std::cos(i*2*PI/32),std::sin(i*2*PI/32)};
        const Vector2 future = room.move(start,Vector2Scale(move,player.getSpeed()*0.25f),14);
        const float distance = Vector2Distance(future,target.getPosition());
        float score = -std::abs(distance-desired)*0.7f + Vector2DotProduct(move,Vector2Normalize(tangent))*12;
        if (Vector2Distance(start,future) < player.getSpeed()*0.20f) score -= 100;
        for (const Enemy& enemy : room.getEnemies())
        {
            const float safe = enemy.getType() == EnemyType::Boss ? 150 : 120;
            score -= std::max(0.0f,safe-Vector2Distance(future,enemy.getPosition()))*3;
        }
        for (const EnemyShot& shot : room.getEnemyShots())
        {
            const Vector2 next = Vector2Add(shot.position,Vector2Scale(shot.velocity,0.20f));
            score -= std::max(0.0f,65-Vector2Distance(future,next))*4;
        }
        for (const Spikes& trap : room.spikes)
            if (!trap.disabled && CheckCollisionCircleRec(future,24,trap.bounds)) score -= 200;
        if (score > bestScore) { bestScore = score; best = move; }
    }
    return best;
}

int main()
{
    Game game;
    game.startNewGame();
    game.dungeon = Dungeon(42);
    const int route[] = {2,3,9,3,4,5,10,5,4,3,2,8,2,3,4,5,6,11,6,7,12};
    std::size_t destination = 0;
    const float dt = 1.0f/60;
    bool talked = false;
    for (int frame = 0; frame < 120000 && game.state != GameState::Victory && game.state != GameState::Defeat; ++frame)
    {
        game.input = {};
        if (game.state == GameState::Attributes)
        {
            if (game.player.getAttributePoints() > 0) { game.selectedOption = 1; game.input.confirm = true; }
            else game.input.escape = true;
            game.update(dt);
            continue;
        }
        if (game.state == GameState::Dialogue) { game.input.confirm = true; game.update(dt); continue; }
        Room& room = game.dungeon.currentRoom();
        const Vector2 position = game.player.getPosition();
        if (destination < std::size(route) && room.getId() == route[destination])
        {
            std::cout << "Sala " << room.getId() << ", vida " << game.player.getHealth() << ", nivel " << game.player.getLevel() << '\n';
            ++destination;
        }
        const Enemy* nearest = nullptr;
        float distance = 1e20f;
        for (const Enemy& enemy : room.getEnemies())
            if (enemy.isAlive() && Vector2Distance(position,enemy.getPosition()) < distance)
            { distance = Vector2Distance(position,enemy.getPosition()); nearest = &enemy; }
        Vector2 goal = position;
        if (nearest)
        {
            if (game.combat.getWeapon().category != WeaponCategory::Ranged) game.input.switchWeapon = true;
            game.input.mouse = GetWorldToScreen2D(nearest->getPosition(),game.camera);
            game.input.attack = true;
            if (distance > 360 || !room.lineClear(position,nearest->getPosition(),4))
                game.input.movement = navigate(room,position,nearest->getPosition());
            else game.input.movement = evade(room,game.player,*nearest);
        }
        else
        {
            bool selected = false;
            if (room.getId() == 8 && !talked)
            {
                goal = room.getNpc().position;
                selected = true;
                if (Vector2Distance(goal,position) < 55) { game.input.interact = true; talked = true; }
            }
            if (!selected && room.hasKey()) { goal = room.getKeyPosition(); selected = true; }
            if (!selected)
                for (const Item& item : room.getItems())
                    if ((item.type == ItemType::HealthKit && game.player.getHealth() < game.player.getMaxHealth()) ||
                        (item.type == ItemType::Weapon && (item.weapon == WeaponType::Spear || item.weapon == WeaponType::Shotgun) &&
                         item.weapon != game.combat.getMeleeWeapon() && item.weapon != game.combat.getRangedWeapon()))
                    {
                        goal = item.position;
                        selected = true;
                        if (item.type == ItemType::Weapon && Vector2Distance(goal,position) < 40) game.input.interact = true;
                        break;
                    }
            if (!selected && room.hasPendingWaves()) { goal = room.getSpawn(); selected = true; }
            if (!selected && destination < std::size(route))
            {
                // Uma luta pode levar de volta a uma porta; retoma a rota a partir da sala atual.
                int previous[13] = {};
                std::queue<int> pending;
                previous[room.getId()] = room.getId(); pending.push(room.getId());
                while (!pending.empty() && previous[route[destination]] == 0)
                {
                    const int id = pending.front(); pending.pop();
                    for (const Connection& door : game.dungeon.findRoom(id)->getConnections())
                        if ((!door.locked || game.dungeon.hasKey()) && previous[door.to] == 0)
                        { previous[door.to] = id; pending.push(door.to); }
                }
                int next = route[destination];
                assert(previous[next] != 0);
                while (previous[next] != room.getId()) next = previous[next];
                for (const Connection& door : room.getConnections())
                    if (door.to == next) goal = {door.door.x+door.door.width/2,door.door.y+door.door.height/2};
            }
            game.input.movement = navigate(room,position,goal);
        }
        game.update(dt);
        if (frame == 119999) std::cerr << "Limite: sala " << room.getId() << " pos " << position.x << ',' << position.y << " inimigos " << room.getEnemies().size() << "\n";
    }
    std::cout << "Estado " << static_cast<int>(game.state) << ", vida " << game.player.getHealth() << ", mortes " << game.player.getDefeatedCount() << ", tempo " << game.elapsedTime << "\n";
    assert(game.state == GameState::Victory);
    assert(game.dungeon.getDiscoveredCount() == 12 && game.dungeon.hasKey() && talked);
    assert(game.player.getDefeatedCount() >= 10 && game.player.getLevel() > 1 && game.player.getDistance() > 10000);
    game.input = {}; game.input.confirm = true; game.update(0);
    assert(game.state == GameState::Playing && game.player.getLevel() == 1);
    game.dungeon.currentRoom().getEnemies().emplace_back(Vector2{game.player.getPosition().x+30,game.player.getPosition().y});
    game.input = {};
    for (int i = 0; i < 1800 && game.state == GameState::Playing; ++i) game.update(dt);
    assert(game.state == GameState::Defeat);
    game.input.confirm = true; game.update(0);
    assert(game.state == GameState::Playing && game.player.getHealth() == 60 && game.player.getDistance() == 0);
    std::cout << "OK: partida completa por entradas reais, 12 salas, chave, armas, NPC, chefe, derrota e reinicios.\n";
}
