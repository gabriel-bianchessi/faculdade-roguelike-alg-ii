#include "Item.h"
#include "Combat.h"
#include "Player.h"
#include "Room.h"
#include "raymath.h"

#include <cassert>
#include <cmath>
#include <iostream>

void consumables()
{
    Room room;
    Player player({200, 300});
    Combat combat;
    auto& items = room.getItems();
    items.push_back({ItemType::HealthKit, player.getPosition()});
    collectItems(room, player, combat, false);
    assert(items.size() == 1 && player.getItemsCollected() == 0);
    player.takeDamage(10);
    collectItems(room, player, combat, false);
    assert(player.getHealth() == 60 && items.empty());
    assert(player.getItemsCollected() == 1);
    player.update({}, 0.51f, room);
    player.takeDamage(40);
    items.push_back({ItemType::HealthKit, player.getPosition()});
    collectItems(room, player, combat, false);
    assert(player.getHealth() == 50);
    items.push_back({ItemType::Shield, player.getPosition()});
    collectItems(room, player, combat, false);
    assert(player.getShield() == 30);
    player.update({}, 0.51f, room);
    player.takeDamage(10);
    assert(player.getShield() == 20 && player.getHealth() == 50);
    items.push_back({ItemType::Shield, player.getPosition()});
    collectItems(room, player, combat, false);
    assert(player.getShield() == 30); // Renova, sem somar 30 aos 20 restantes.
    player.update({}, 0.51f, room);
    player.takeDamage(35);
    assert(player.getShield() == 0 && player.getHealth() == 45);
    assert(!player.takeDamage(10)); // Escudo tambem respeita invulnerabilidade.

    items.push_back({ItemType::DamageBoost, player.getPosition(), WeaponType::Sword, 25});
    collectItems(room, player, combat, false);
    assert(player.getDamageMultiplier() == 1.25f);
    player.update({}, 9, room);
    assert(std::abs(player.getBonusRemaining() - 1) < 0.001f);
    items.push_back({ItemType::DamageBoost, player.getPosition(), WeaponType::Sword, 25});
    collectItems(room, player, combat, false);
    assert(player.getBonusRemaining() == 10 && player.getDamageMultiplier() == 1.25f);
    player.update({}, 10.1f, room);
    assert(player.getBonusRemaining() == 0 && player.getDamageMultiplier() == 1);
    assert(player.getItemsCollected() == 6);
}

void inventory()
{
    Room room;
    Player player({200, 300});
    Combat combat;
    room.getItems().push_back({ItemType::Weapon, player.getPosition(), WeaponType::Spear});
    collectItems(room, player, combat, false);
    assert(combat.getMeleeWeapon() == WeaponType::Sword);
    collectItems(room, player, combat, true);
    assert(combat.getMeleeWeapon() == WeaponType::Spear);
    assert(combat.getRangedWeapon() == WeaponType::Pistol);
    assert(combat.getWeapon().type == WeaponType::Spear);
    assert(room.getItems().size() == 1);
    assert(room.getItems()[0].weapon == WeaponType::Sword);
    assert(player.getItemsCollected() == 1);
    collectItems(room, player, combat, false);
    assert(combat.getMeleeWeapon() == WeaponType::Spear); // Nao recolhe a descartada.
    room.getItems().push_back({ItemType::Weapon, {205, 300}, WeaponType::Shotgun});
    player.enterRoom({205, 300});
    collectItems(room, player, combat, true);
    assert(combat.getRangedWeapon() == WeaponType::Shotgun);
    assert(combat.getMeleeWeapon() == WeaponType::Spear);
    assert(combat.getWeapon().type == WeaponType::Shotgun);
    combat.switchWeapon();
    assert(combat.getWeapon().type == WeaponType::Spear);
    assert(room.getItems().size() == 2);
    assert(room.getItems()[1].weapon == WeaponType::Pistol);
    assert(player.getItemsCollected() == 2);
    // Uma nova interacao permite trocar de volta com a arma descartada.
    player.enterRoom({200, 300});
    collectItems(room, player, combat, true);
    assert(combat.getMeleeWeapon() == WeaponType::Sword);
    assert(player.getItemsCollected() == 3);
    player.enterRoom({700, 700});
    collectItems(room, player, combat, true);
    assert(player.getItemsCollected() == 3);
}

void arsenal()
{
    Room room;
    Player player({200, 300});
    Combat combat;
    combat.replaceWeapon(WeaponType::Spear);
    room.getEnemies().emplace_back(Vector2{310, 300}); // Fora do alcance da espada.
    room.getEnemies().emplace_back(Vector2{250, 345}); // Fora da largura da lanca.
    combat.update(0, true, player, room);
    assert(room.getEnemies()[0].getHealth() == 12);
    assert(room.getEnemies()[1].getHealth() == 40);
    combat.update(0.1f, true, player, room);
    assert(room.getEnemies()[0].getHealth() == 12);
    combat.update(0.54f, true, player, room);
    assert(room.getEnemies()[0].getHealth() == 12);
    combat.update(0.01f, true, player, room);
    assert(room.getEnemies().size() == 1);

    Room open(5000, 5000);
    Combat shotgunCombat;
    shotgunCombat.replaceWeapon(WeaponType::Shotgun);
    player.boostDamage(0.25f, 10);
    shotgunCombat.update(0, true, player, open);
    assert(shotgunCombat.getProjectiles().size() == 5);
    const auto& shots = shotgunCombat.getProjectiles();
    assert(shots[0].velocity.y < shots[1].velocity.y);
    assert(shots[1].velocity.y < 0 && shots[2].velocity.y == 0);
    assert(shots[3].velocity.y > 0 && shots[4].velocity.y > shots[3].velocity.y);
    for (const Projectile& shot : shots)
    {
        assert(shot.damage == 7.5f);
        assert(std::abs(Vector2Length(shot.velocity) - 650) < 0.01f);
    }
    player.update({}, 10, room); // O dano ja disparado conserva o bonus original.
    shotgunCombat.update(0.05f, false, player, open);
    assert(shotgunCombat.getProjectiles()[0].damage == 7.5f);
    shotgunCombat.update(0.84f, true, player, open);
    assert(shotgunCombat.getProjectiles().size() == 5);
    shotgunCombat.update(0.01f, true, player, open);
    assert(shotgunCombat.getProjectiles().size() == 10);
    assert(shotgunCombat.getProjectiles().back().damage == 6);

    Room targetRoom;
    Combat hit;
    Player boosted({200, 300});
    boosted.boostDamage(0.25f, 10);
    hit.replaceWeapon(WeaponType::Shotgun);
    targetRoom.getEnemies().emplace_back(Vector2{240, 300});
    hit.update(0, true, boosted, targetRoom);
    hit.update(0.1f, false, boosted, targetRoom);
    assert(targetRoom.getEnemies().size() == 1);
    assert(targetRoom.getEnemies()[0].getHealth() == 2.5f); // 5 x 7,5, sem arredondar.
    assert(hit.getProjectiles().empty());
}

int main()
{
    consumables();
    inventory();
    arsenal();
    std::cout << "OK: cura, escudo, bonus, coletas, troca unica, lanca e escopeta.\n";
}
