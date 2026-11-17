#include "Game.h"
#include <cassert>
#include <iostream>

int main()
{
    SetTraceLogLevel(LOG_WARNING);
    // O atlas e os 30 PNGs precisam existir na entrega, sem legendas ou grade.
    Image atlas = LoadImage("resources/player/knight.png");
    assert(IsImageValid(atlas) && atlas.width == 768 && atlas.height == 1024);
    UnloadImage(atlas);
    const char* names[] = {"idle","walk-down","walk-left","walk-right","walk-up","attack","hurt","death"};
    for (int row=0; row<8; ++row)
        for (int frame=0; frame<(row == 6 ? 2 : 4); ++frame)
        {
            Image image = LoadImage(TextFormat("resources/player/frames/%s-%i.png",names[row],frame));
            assert(IsImageValid(image) && image.width == 192 && image.height == 128);
            const Rectangle content = GetImageAlphaBorder(image,0.5f);
            assert(content.width > 70 && content.height > 50);
            assert(content.x > 0 && content.y > 0);
            assert(content.x+content.width < image.width && content.y+content.height < image.height);
            UnloadImage(image);
        }
    Room room;
    room.setObstacles({});
    Player idle({500,400});
    idle.update({},0.19f,room);
    assert(idle.getAnimation() == PlayerAnimation::Idle && idle.getAnimationFrame() == 1);
    const Vector2 directions[] = {{0,1},{-1,0},{1,0},{0,-1}};
    const PlayerAnimation animations[] = {PlayerAnimation::WalkDown,PlayerAnimation::WalkLeft,
        PlayerAnimation::WalkRight,PlayerAnimation::WalkUp};
    for (int i=0; i<4; ++i)
    {
        Player walker({500,400});
        walker.update(directions[i],0.01f,room);
        assert(walker.getAnimation() == animations[i]);
        walker.update(directions[i],0.11f,room);
        assert(walker.getAnimationFrame() == 1);
        walker.update({},0.5f,room);
        assert(walker.getAnimation() == (i == 0 ? PlayerAnimation::Idle : animations[i]));
        if (i != 0) assert(walker.getAnimationFrame() == 0); // Nao anda parado.
    }
    Player swordsman({500,400});
    Combat combat;
    combat.update(0,true,swordsman,room);
    assert(swordsman.usesSwordAttackSprite());
    for (int frame=1; frame<4; ++frame)
    {
        swordsman.update({},0.075f,room);
        assert(swordsman.getAnimation() == PlayerAnimation::Attack);
        assert(swordsman.getAnimationFrame() == frame);
    }
    swordsman.update({},0.1f,room);
    assert(!swordsman.usesSwordAttackSprite());
    assert(swordsman.takeDamage(10));
    assert(swordsman.getAnimation() == PlayerAnimation::Hurt);
    swordsman.update({},0.1f,room);
    assert(swordsman.getAnimationFrame() == 1);
    assert(!swordsman.takeDamage(10)); // Invulnerabilidade nao reinicia a animacao.
    swordsman.update({},0.2f,room);
    assert(swordsman.getAnimation() == PlayerAnimation::Idle);

    Game game;
    game.startNewGame();
    game.player.playSwordAnimation();
    game.changeState(GameState::Paused);
    game.update(5);
    assert(game.player.getAnimationFrame() == 0 && game.player.usesSwordAttackSprite());
    assert(game.player.takeDamage(100));
    game.changeState(GameState::Defeat);
    const float elapsed = game.elapsedTime;
    game.update(0.25f);
    assert(game.player.getAnimation() == PlayerAnimation::Death && game.player.getAnimationFrame() == 2);
    game.update(5);
    assert(game.player.getAnimationFrame() == 3 && game.elapsedTime == elapsed);
    game.startNewGame();
    assert(game.player.getAnimation() == PlayerAnimation::Idle && game.player.getAnimationFrame() == 0);
    assert(!game.player.usesSwordAttackSprite());
    std::cout << "OK: atlas, 30 quadros, quatro direcoes, repouso, espada, dano, pausa, morte e reinicio.\n";
}
