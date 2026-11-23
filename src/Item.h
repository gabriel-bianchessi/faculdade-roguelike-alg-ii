#pragma once

#include "Config.h"
#include "Weapon.h"

class Room;
class Player;
class Combat;

enum class ItemType { HealthKit, Shield, DamageBoost, Weapon, Experience };

struct Item
{
    ItemType type;
    Vector2 position;
    WeaponType weapon = WeaponType::Sword;
    int amount = Config::kitHeal;
    float duration = Config::damageBonusDuration;
};

void collectItems(Room& room, Player& player, Combat& combat, bool interact);
const Item* nearbyWeapon(const Room& room, const Player& player, const Combat& combat);
void drawItems(const Room& room, const Player& player, const Combat& combat);

const char* collectionName(int kind);
