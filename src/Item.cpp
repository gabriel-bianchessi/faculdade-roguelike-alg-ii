#include "Item.h"

#include "Room.h"
#include "Player.h"
#include "Combat.h"
#include "WeaponVisual.h"
#include "raymath.h"

#include <algorithm>

const Item* nearbyWeapon(const Room& room, const Player& player, const Combat& combat)
{
    const Item* nearest = nullptr;
    float distance = Config::weaponPickupRadius;
    for (const Item& item : room.getItems())
    {
        if (item.type != ItemType::Weapon) continue;
        const WeaponType owned = weaponConfig(item.weapon).category == WeaponCategory::Melee ?
            combat.getMeleeWeapon() : combat.getRangedWeapon();
        if (owned == item.weapon) continue;
        const float candidate = Vector2Distance(item.position, player.getPosition());
        if (candidate <= distance)
        {
            nearest = &item;
            distance = candidate;
        }
    }
    return nearest;
}

void collectItems(Room& room, Player& player, Combat& combat, bool interact)
{
    auto& items = room.getItems();
    std::erase_if(items, [&](const Item& item)
    {
        if (item.type == ItemType::Weapon ||
            Vector2Distance(item.position, player.getPosition()) > Config::playerHalfSize + Config::itemRadius)
            return false;
        switch (item.type)
        {
            case ItemType::HealthKit:
                if (player.heal(item.amount) == 0) return false;
                break;
            case ItemType::Shield:
                player.grantShield(item.amount);
                break;
            case ItemType::DamageBoost:
                player.boostDamage(item.amount / 100.0f, item.duration);
                break;
            case ItemType::Experience:
                player.gainExperience(item.amount);
                break;
            case ItemType::Weapon: return false;
        }
        player.recordCollection(item.type == ItemType::Experience ? 7 : static_cast<int>(item.type));
        return true;
    });
    if (!interact) return;
    const Item* nearest = nearbyWeapon(room, player, combat);
    if (!nearest) return;
    const std::size_t index = static_cast<std::size_t>(nearest - items.data());
    const Item picked = *nearest;
    const WeaponType dropped = combat.replaceWeapon(picked.weapon);
    items.erase(items.begin() + index);
    items.push_back({ItemType::Weapon, picked.position, dropped});
    player.recordCollection(3+static_cast<int>(picked.weapon));
    // Uma tecla realiza uma unica troca. A arma descartada so pode ser recolhida
    // em outra interacao, mesmo estando na mesma posicao da arma original.
}

void drawItems(const Room& room, const Player& player, const Combat& combat)
{
    for (const Item& item : room.getItems())
    {
        if (!room.isVisible(item.position)) continue;
        const Vector2 p = item.position;
        switch (item.type)
        {
            case ItemType::HealthKit:
                DrawRectangleRec({p.x - 12, p.y - 12, 24, 24}, DARKGREEN);
                DrawLineEx({p.x - 7, p.y}, {p.x + 7, p.y}, 4, WHITE);
                DrawLineEx({p.x, p.y - 7}, {p.x, p.y + 7}, 4, WHITE);
                break;
            case ItemType::Shield:
                DrawCircleV(p, 12, BLUE);
                DrawCircleLines(static_cast<int>(p.x), static_cast<int>(p.y), 9, SKYBLUE);
                break;
            case ItemType::DamageBoost:
                DrawRectanglePro({p.x, p.y, 20, 20}, {10, 10}, 45, PURPLE);
                DrawText("+", static_cast<int>(p.x - 5), static_cast<int>(p.y - 9), 18, WHITE);
                break;
            case ItemType::Experience:
                DrawPoly(p,4,13,0,SKYBLUE);
                DrawPoly(p,4,7,0,WHITE);
                break;
            case ItemType::Weapon:
            {
                DrawEllipse(p.x,p.y+10,24,7,Fade(BLACK,0.35f));
                drawWeapon(item.weapon,{p.x-8,p.y+3},{0.92f,-0.38f},0.85f);
                break;
            }
        }
    }
    const Item* weapon = nearbyWeapon(room, player, combat);
    if (weapon)
        DrawText(TextFormat("E: %s", weaponName(weapon->weapon)),
                 static_cast<int>(weapon->position.x - 35), static_cast<int>(weapon->position.y - 32),
                 16, Config::text);
}

const char* collectionName(int kind)
{
    const char* names[] = {"Kit de vida", "Escudo", "Furia", "Espada", "Pistola",
        "Lanca", "Escopeta", "Cristal de XP", "Chave"};
    return kind >= 0 && kind < 9 ? names[kind] : "";
}
