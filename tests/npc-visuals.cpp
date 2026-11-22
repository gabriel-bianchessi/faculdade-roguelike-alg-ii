#include "Game.h"
#include <cassert>
#include <cmath>
#include <filesystem>
#include <iostream>

int main()
{
    SetTraceLogLevel(LOG_WARNING);
    for (const char* type : {"watchman","healer"})
    {
        const auto root = std::filesystem::path("resources/npcs") / type;
        Image atlas=LoadImage((root / "atlas.png").string().c_str());
        assert(IsImageValid(atlas) && atlas.width == 1536 && atlas.height == 1536);
        UnloadImage(atlas);
        int count=0;
        for (const auto& entry : std::filesystem::directory_iterator(root / "frames"))
        {
            Image frame=LoadImage(entry.path().string().c_str());
            assert(IsImageValid(frame) && frame.width == 384 && frame.height == 384);
            Color* pixels=LoadImageColors(frame);
            int visible=0;
            for (int y=0;y<384;++y)
                for (int x=0;x<384;++x)
                {
                    const unsigned char alpha=pixels[y*384+x].a;
                    assert(alpha == 0 || alpha == 255);
                    if (alpha) { ++visible;assert(x>0 && x<383 && y>0 && y<383); }
                }
            assert(visible>10000);
            UnloadImageColors(pixels);UnloadImage(frame);++count;
        }
        assert(count == 16);
    }
    Npc guard{NpcType::Watchman};
    guard.update(0.32f);
    assert(guard.animationRow == 0 && guard.getAnimationFrame() == 2);
    for (int line=0;line<3;++line)
    {
        guard.update(0.1f,true,line);
        assert(guard.animationRow == line+1 && guard.getAnimationFrame() == 0);
    }
    guard.update(0.1f);
    assert(guard.animationRow == 0);
    Game game;
    game.startNewGame();
    game.dungeon.currentIndex=7;
    Npc& healer=game.dungeon.currentRoom().getNpc();
    game.player.takeDamage(20);
    game.dungeon.finishDialogue(game.player);
    assert(game.player.getHealth() == 60 && healer.healingAnimationRemaining == 1.28f);
    game.input = {};
    game.update(0.2f);
    assert(healer.animationRow == 2);
    game.changeState(GameState::Paused);
    const float remaining=healer.healingAnimationRemaining;
    game.update(10);
    assert(healer.healingAnimationRemaining == remaining);
    game.changeState(GameState::Playing);
    game.update(0.5f);
    assert(healer.animationRow == 3);
    game.update(0.7f);
    assert(healer.animationRow == 0 && healer.healingAnimationRemaining == 0);
    game.dungeon.finishDialogue(game.player);
    assert(healer.healingAnimationRemaining == 0); // Cura e efeito ocorrem uma vez.
    game.startNewGame();
    assert(!game.dungeon.findRoom(8)->getNpc().healingUsed);
    std::cout << "OK: 32 quadros de NPCs, transparencia, dialogos, cura unica, pausa e reinicio.\n";
}
