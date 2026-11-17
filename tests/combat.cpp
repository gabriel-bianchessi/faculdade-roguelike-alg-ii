#include "Combat.h"
#include "Player.h"
#include "Room.h"
#include "raymath.h"

#include <cassert>
#include <cmath>
#include <iostream>

void melee()
{
    Room room;
    Player player({200, 300});
    Combat combat;
    room.getEnemies().emplace_back(Vector2{250, 300});
    room.getEnemies().emplace_back(Vector2{250, 325});
    room.getEnemies().emplace_back(Vector2{150, 300});
    room.getEnemies().emplace_back(Vector2{300, 300});
    combat.update(0, true, player, room);
    assert(room.getEnemies()[0].getHealth() == 20);
    assert(room.getEnemies()[1].getHealth() == 20);
    assert(room.getEnemies()[2].getHealth() == 40);
    assert(room.getEnemies()[3].getHealth() == 40);
    combat.update(0.1f, true, player, room);
    assert(room.getEnemies()[0].getHealth() == 20);
    combat.switchWeapon();
    combat.switchWeapon();
    combat.update(0.1f, true, player, room);
    assert(room.getEnemies()[0].getHealth() == 20); // Q nao reinicia a cadencia.
    combat.update(0.2f, true, player, room);
    assert(room.getEnemies().size() == 2); // Remocao de mortes consecutivas.

    Room narrow(640, 800);
    Player besideWall({185, 300});
    Combat blocked;
    narrow.getEnemies().emplace_back(Vector2{260, 300});
    blocked.update(0, true, besideWall, narrow);
    assert(narrow.getEnemies()[0].getHealth() == 40);

    Player diagonal({200, 300});
    diagonal.aimAt({300, 400});
    room.getEnemies().clear();
    Combat angled;
    room.getEnemies().emplace_back(Vector2{245, 345});
    angled.update(0, true, diagonal, room);
    assert(room.getEnemies()[0].getHealth() == 20);
}

void projectiles()
{
    Room room;
    Player player({200, 300});
    Combat combat;
    combat.switchWeapon();
    room.getEnemies().emplace_back(Vector2{260, 300});
    room.getEnemies().emplace_back(Vector2{320, 300});
    combat.update(0, true, player, room);
    combat.update(0.2f, false, player, room);
    assert(combat.getProjectiles().empty());
    assert(room.getEnemies()[0].getHealth() == 32);
    assert(room.getEnemies()[1].getHealth() == 40);
    combat.update(0, true, player, room);
    assert(combat.getProjectiles().empty());
    combat.update(0.1f, true, player, room);
    assert(room.getEnemies()[0].getHealth() == 24);

    room.getEnemies().clear();
    Combat wall;
    wall.switchWeapon();
    room.getEnemies().emplace_back(Vector2{490, 300});
    wall.update(0, true, player, room);
    wall.update(1, false, player, room); // Tiro passa a espessura inteira em um quadro.
    assert(wall.getProjectiles().empty());
    assert(room.getEnemies()[0].getHealth() == 40);

    Combat expired;
    Room large(5000, 5000);
    expired.switchWeapon();
    Player farAway({100, 100});
    expired.update(0, true, farAway, large);
    expired.update(2.1f, false, farAway, large);
    assert(expired.getProjectiles().empty());

    for (int fps : {15, 30, 60, 144, 240})
    {
        Combat cadence;
        cadence.switchWeapon();
        for (int frame = 0; frame < fps; ++frame)
            cadence.update(1.0f / fps, true, farAway, large);
        assert(cadence.getProjectiles().size() == 4);
    }
}

void dashAndDamage()
{
    Room room;
    Player player({100, 100});
    assert(player.takeDamage(10));
    assert(!player.takeDamage(10));
    player.update({}, 0.49f, room);
    assert(!player.takeDamage(10));
    player.update({}, 0.011f, room);
    assert(player.takeDamage(10));
    assert(player.getHealth() == 40);

    Player dash({100, 100});
    dash.update({1, 0}, 0.01f, room, true);
    assert(dash.isInvulnerable());
    assert(!dash.takeDamage(10));
    dash.update({}, 0.10f, room);
    assert(dash.isInvulnerable());
    dash.update({}, 0.02f, room);
    assert(!dash.isInvulnerable());
    assert(dash.takeDamage(10));
    dash.update({}, 0.112f, room);
    assert(std::abs(dash.getPosition().x - 292) < 0.01f);
    const Vector2 position = dash.getPosition();
    dash.update({}, 0.1f, room, true);
    assert(Vector2Distance(position, dash.getPosition()) < 0.01f); // Em recarga.

    Player overlapping({100, 100});
    assert(overlapping.takeDamage(10));
    overlapping.update({}, 0.4f, room);
    overlapping.update({}, 0.01f, room, true);
    overlapping.update({}, 0.12f, room);
    assert(!overlapping.isInvulnerable()); // Os dois tempos nao se somam.

    for (int fps : {15, 30, 60, 144, 240})
    {
        Player runner({200, 300});
        for (int frame = 0; frame < fps; ++frame)
            runner.update(frame == 0 ? Vector2{1, 0} : Vector2{}, 1.0f / fps, room, frame == 0);
        assert(std::abs(runner.getPosition().x - 392) < 0.05f);
        Player blocked({390, 300});
        blocked.update({1, 0}, 0.8f, room, true);
        assert(blocked.getPosition().x <= room.getSolids()[4].x - Config::playerHalfSize);
    }
}

void enemy()
{
    Room room;
    Player player({200, 300});
    Enemy chaser({230, 300});
    chaser.update(0, room, player);
    assert(player.getHealth() == 50);
    for (int i = 0; i < 79; ++i)
    {
        player.update({}, 0.01f, room);
        chaser.update(0.01f, room, player);
    }
    assert(player.getHealth() == 50);
    player.update({}, 0.02f, room);
    chaser.update(0.02f, room, player);
    assert(player.getHealth() == 40);
    chaser.takeDamage(100);
    chaser.update(10, room, player);
    assert(player.getHealth() == 40);

    Player acrossWall({500, 300});
    Enemy blocked({390, 300});
    blocked.update(10, room, acrossWall);
    assert(blocked.getPosition().x <= room.getSolids()[4].x - Config::enemyHalfSize);
}

int main()
{
    melee();
    projectiles();
    dashAndDamage();
    enemy();
    std::cout << "OK: golpes, cadencia, tiros, paredes, mortes, dash, invulnerabilidade e perseguidor.\n";
}
