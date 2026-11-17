#include "Game.h"
#include <cassert>
#include <iostream>

int main()
{
    SetTraceLogLevel(LOG_WARNING);
    const char* names[]={"chaser","shooter","patrol"};
    const char* animations[]={"idle","walk-down","walk-left","walk-right","walk-up","attack","hurt","death"};
    for (const char* name : names)
    {
        Image atlas=LoadImage(TextFormat("resources/enemies/%s/atlas.png",name));
        assert(IsImageValid(atlas) && atlas.width==1024 && atlas.height==2048);
        UnloadImage(atlas);
        for (int row=0;row<8;++row)
            for (int frame=0;frame<(row==6 ? 2 : 4);++frame)
            {
                Image image=LoadImage(TextFormat("resources/enemies/%s/frames/%s-%i.png",name,animations[row],frame));
                assert(IsImageValid(image) && image.width==256 && image.height==256);
                const Rectangle bounds=GetImageAlphaBorder(image,0.5f);
                assert(bounds.width>90 && bounds.height>50);
                assert(bounds.x>0 && bounds.y>0 && bounds.x+bounds.width<256 && bounds.y+bounds.height<256);
                UnloadImage(image);
            }
    }
    Room room;
    room.setObstacles({});
    const Vector2 targets[]={{500,600},{300,400},{700,400},{500,200}};
    const EnemyAnimation directions[]={EnemyAnimation::WalkDown,EnemyAnimation::WalkLeft,
        EnemyAnimation::WalkRight,EnemyAnimation::WalkUp};
    for (EnemyType type : {EnemyType::Chaser,EnemyType::Shooter,EnemyType::Patrol})
        for (int i=0;i<4;++i)
        {
            Player target(targets[i]);
            Enemy enemy({500,400},type);
            enemy.attackCooldown=100; // Isola a animacao de caminhada.
            enemy.update(0.01f,room,target);
            assert(enemy.getAnimation()==directions[i]);
            enemy.update(0.11f,room,target);
            assert(enemy.getAnimationFrame()==1);
            assert(enemy.getHealth()==40 && enemy.getBounds().width==32);
        }
    Player target({500,400});
    Enemy shooter({650,400},EnemyType::Shooter);
    shooter.update(0.01f,room,target);
    assert(shooter.getAnimation()==EnemyAnimation::Attack && room.getEnemyShots().size()==1);
    shooter.update(0.15f,room,target);
    assert(shooter.getAnimationFrame()==2 && shooter.getPosition().x<649);
    assert(room.getEnemyShots().size()==1); // Animacao nao muda cadencia.
    shooter.takeDamage(8);
    assert(shooter.getAnimation()==EnemyAnimation::Hurt && shooter.getHealth()==32);
    shooter.update(0.1f,room,target);
    assert(shooter.getAnimationFrame()==1);
    for (EnemyType type : {EnemyType::Chaser,EnemyType::Patrol})
    {
        Player victim({500,400});
        Enemy enemy({525,400},type);
        enemy.update(0.01f,room,victim);
        assert(enemy.getAnimation()==EnemyAnimation::Attack && victim.getHealth()==50);
    }

    Game game;
    game.startNewGame();
    Room& active=game.dungeon.currentRoom();
    active.getEnemies().emplace_back(Vector2{500,400});
    active.getEnemies().back().takeDamage(100);
    game.combat.update(0,false,game.player,active);
    assert(active.getEnemies().empty() && active.getDyingEnemies().size()==1 && !active.hasLivingEnemies());
    assert(game.player.getDefeatedCount()==1 && game.player.getExperience()==20);
    game.combat.update(0,false,game.player,active);
    assert(game.player.getDefeatedCount()==1 && active.getDyingEnemies().size()==1);
    assert(active.getDyingEnemies()[0].getAnimation()==EnemyAnimation::Death);
    game.update(0.13f);
    assert(active.getDyingEnemies()[0].getAnimationFrame()==1);
    game.changeState(GameState::Paused); game.update(5);
    assert(active.getDyingEnemies()[0].getAnimationFrame()==1);
    game.changeState(GameState::Attributes); game.update(5);
    assert(active.getDyingEnemies()[0].getAnimationFrame()==1);
    game.changeState(GameState::Playing);
    // A animacao de morte nao bloqueia a passagem; na sala inativa fica congelada.
    const Connection door=active.getConnections()[0];
    game.player.enterRoom({door.door.x+door.door.width/2,door.door.y+door.door.height/2});
    game.update(0.01f);
    assert(game.dungeon.currentRoom().getId()==2);
    assert(active.getDyingEnemies()[0].getAnimationFrame()==1);
    game.update(0.1f);
    assert(active.getDyingEnemies()[0].getAnimationFrame()==1);
    active.updateEnemyDeaths(0.7f);
    assert(active.getDyingEnemies().empty());
    assert(game.player.getDefeatedCount()==1);
    game.startNewGame();
    assert(game.dungeon.currentRoom().getDyingEnemies().empty());
    std::cout << "OK: tres atlas, 90 quadros, direcoes, ataques, dano, mortes, XP unico, portas, pausa e reinicio.\n";
}
