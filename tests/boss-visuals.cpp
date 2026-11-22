#include "Game.h"
#include <cassert>
#include <iostream>

int main()
{
    SetTraceLogLevel(LOG_WARNING);
    const char* bossNames[]={"idle","walk-down","walk-left","walk-right","walk-up","charge-warning",
        "charge","fan-warning","fan-attack","recovery","hurt","death"};
    const char* shotNames[]={"north","north-east","east","south-east","south","south-west","west","north-west"};
    for (bool boss : {true,false})
    {
        const char* root=boss ? "resources/enemies/boss" : "resources/projectiles/hostile";
        Image atlas=LoadImage(TextFormat("%s/atlas.png",root));
        assert(IsImageValid(atlas) && atlas.width==1024 && atlas.height==(boss ? 3072 : 2048));
        UnloadImage(atlas);
        for (int row=0;row<(boss ? 12 : 8);++row)
            for (int frame=0;frame<4;++frame)
            {
                Image image=LoadImage(TextFormat("%s/frames/%s-%i.png",root,boss ? bossNames[row] : shotNames[row],frame));
                assert(IsImageValid(image) && image.width==256 && image.height==256);
                const Rectangle area=GetImageAlphaBorder(image,0.5f);
                assert(area.width>20 && area.height>20);
                assert(area.x>0 && area.y>0 && area.x+area.width<256 && area.y+area.height<256);
                UnloadImage(image);
            }
    }
    const Vector2 directions[]={{0,-100},{100,-100},{100,0},{100,100},{0,100},{-100,100},{-100,0},{-100,-100}};
    Room room;
    room.setObstacles({});
    Player player({100,100});
    for (int row=0;row<8;++row)
    {
        EnemyShot shot{{600,400},directions[row],20};
        assert(shot.getSpriteRow()==row && shot.getSpriteFrame()==0);
        room.getEnemyShots().push_back(shot);
    }
    room.updateEnemyShots(0.17f,player);
    assert(room.getEnemyShots().size()==8);
    for (int row=0;row<8;++row)
    {
        const EnemyShot& shot=room.getEnemyShots()[row];
        assert(shot.getSpriteRow()==row && shot.getSpriteFrame()==2 && shot.damage==20);
        assert(shot.remaining<Config::hostileShotLifetime);
    }
    Game game;
    game.startNewGame();
    game.dungeon.currentRoom().getEnemyShots().push_back({{600,400},{100,0}});
    game.update(0.12f);
    const float age=game.dungeon.currentRoom().getEnemyShots()[0].age;
    game.changeState(GameState::Paused);game.update(5);
    assert(game.dungeon.currentRoom().getEnemyShots()[0].age==age);
    game.startNewGame();
    assert(game.dungeon.currentRoom().getEnemyShots().empty());
    std::cout << "OK: 48 quadros do chefe, 32 dos disparos, oito direcoes, animacao, pausa e reinicio.\n";
}
