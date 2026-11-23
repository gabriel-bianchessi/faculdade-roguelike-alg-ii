#include "Game.h"
#include <cassert>
#include <iostream>

int main()
{
    int enemies[3]{}, items[3]{}, chasers[3]{};
    for (int d=0; d<3; ++d)
        for (unsigned int seed=0; seed<100; ++seed)
        {
            Dungeon dungeon(seed,static_cast<Difficulty>(d));
            assert(dungeon.findRoom(9)->hasKey());
            assert(dungeon.findRoom(1)->getEnemies().empty());
            assert(dungeon.findRoom(8)->getEnemies().empty());
            for (int id=2; id<12; ++id)
            {
                Room& room=*dungeon.findRoom(id);
                items[d]+=room.getItems().size();
                const auto count=[&](const std::vector<Enemy>& group) {
                    for (const Enemy& enemy:group)
                    {
                        ++enemies[d];
                        chasers[d]+=enemy.getType()==EnemyType::Chaser;
                    }
                };
                count(room.getEnemies());
                for (const auto& wave:room.waves) count(wave);
            }
        }
    assert(enemies[0]<enemies[1] && enemies[1]<enemies[2]);
    assert(items[0]>items[1] && items[1]>items[2]);
    assert(double(chasers[0])/enemies[0]>double(chasers[1])/enemies[1]);
    assert(double(chasers[1])/enemies[1]>double(chasers[2])/enemies[2]);
    Game game;
    game.input.confirm=true;
    game.update(0);
    assert(game.state==GameState::DifficultySelection);
    game.selectedOption=2;
    game.update(0);
    assert(game.state==GameState::Playing && game.difficulty==Difficulty::Hard);
    std::cout<<"OK: dificuldades, probabilidades em 300 partidas e selecao no menu.\n";
}
