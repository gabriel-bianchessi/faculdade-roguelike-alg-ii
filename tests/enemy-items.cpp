#include "Game.h"
#include <cassert>
#include <iostream>

int main()
{
    Room room;
    room.setObstacles({});
    room.placeKey({200,300});
    for (ItemType type:{ItemType::HealthKit,ItemType::Shield,ItemType::DamageBoost,ItemType::Experience,ItemType::Weapon})
        room.getItems().push_back({type,{200,300}});
    room.getEnemies().emplace_back(Vector2{200,300});
    room.updateItemDestruction(.01f);
    assert(room.getItems().size()==1 && room.getItems()[0].type==ItemType::Weapon && room.hasKey());
    assert(room.destroyedItems.size()==4);
    Player player;
    assert(player.getItemsCollected()==0);
    room.updateItemDestruction(1);
    assert(room.destroyedItems.empty());
    room.getEnemies().clear();
    room.getEnemies().emplace_back(Vector2{200,300},EnemyType::Boss);
    room.getItems().push_back({ItemType::HealthKit,{200,300}});
    room.updateItemDestruction(1);
    assert(room.getItems().size()==2);
    room.getEnemies().clear();
    room.getEnemies().emplace_back(Vector2{200,300});
    room.getEnemies()[0].takeDamage(1000);
    room.updateItemDestruction(1);
    assert(room.getItems().size()==2);
    room.getEnemies().clear();
    room.setObstacles({{210,260,10,80}});
    room.getEnemies().emplace_back(Vector2{200,300});
    room.getItems().push_back({ItemType::Shield,{225,300}});
    room.updateItemDestruction(.01f);
    assert(room.getItems().back().type==ItemType::Shield); // Nao destroi atraves da parede.
    std::cout<<"OK: inimigos destroem consumiveis, preservam armas/chave e respeitam paredes.\n";
}
