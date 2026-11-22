#include "Player.h"
#include "Room.h"
#include "raymath.h"

#include <cassert>
#include <cmath>
#include <cstdio>

namespace
{
    void near(float actual, float expected, float tolerance = 0.02f)
    {
        if (std::abs(actual - expected) >= tolerance)
            std::fprintf(stderr, "Esperado %.4f, obtido %.4f\n", expected, actual);
        assert(std::abs(actual - expected) < tolerance);
    }

    void near(Vector2 actual, Vector2 expected)
    {
        near(actual.x, expected.x);
        near(actual.y, expected.y);
    }

    void assertFree(const Room& room, Vector2 position)
    {
        const float half = Config::playerHalfSize;
        const Rectangle body = {position.x - half, position.y - half, half * 2, half * 2};
        for (const Rectangle& solid : room.getSolids())
            assert(!CheckCollisionRecs(body, solid));
    }
}

int main()
{
    const Room room;
    const float half = Config::playerHalfSize;
    const float wall = Config::wallThickness;
    const Vector2 start = {200, 300};
    const Vector2 directions[] = {
        {1, 0}, {-1, 0}, {0, 1}, {0, -1},
        {1, 1}, {1, -1}, {-1, 1}, {-1, -1}
    };

    Player initial(room.getSpawn());
    assert(initial.getHealth() == 60 && initial.getMaxHealth() == 60);
    near(initial.getSpeed(), 240);
    near(initial.getPosition(), room.getSpawn());
    assertFree(room, initial.getPosition());

    // Um segundo em area livre: mesma distancia axial ou diagonal, em varios FPS.
    const Room movementRoom(2400, 1600);
    const Vector2 movementStart = {350, 400};
    for (int fps : {15, 30, 60, 144, 240})
    {
        for (Vector2 movement : directions)
        {
            Player player(movementStart);
            for (int frame = 0; frame < fps; ++frame)
                player.update(movement, 1.0f / fps, movementRoom);
            near(Vector2Distance(movementStart, player.getPosition()), 240);
            near(player.getPosition(), Vector2Add(movementStart, Vector2Scale(Vector2Normalize(movement), 240)));
            assertFree(movementRoom, player.getPosition());
        }
    }

    // Varredura ate as quatro paredes, incluindo deslocamentos muito grandes.
    const Rectangle bounds = room.getBounds();
    near(room.move(start, {-10000, 0}, half), {wall + half, start.y});
    near(room.move({200, 100}, {10000, 0}, half), {bounds.width - wall - half, 100});
    near(room.move(start, {0, -10000}, half), {start.x, wall + half});
    near(room.move(start, {0, 10000}, half), {start.x, bounds.height - wall - half});

    // Cada obstaculo bloqueia aproximacoes por todas as faces.
    for (std::size_t i = 4; i < room.getSolids().size(); ++i)
    {
        const Rectangle solid = room.getSolids()[i];
        const float cx = solid.x + solid.width / 2;
        const float cy = solid.y + solid.height / 2;
        near(room.move({solid.x - half - 20, cy}, {10000, 0}, half), {solid.x - half, cy});
        near(room.move({solid.x + solid.width + half + 20, cy}, {-10000, 0}, half),
             {solid.x + solid.width + half, cy});
        near(room.move({cx, solid.y - half - 20}, {0, 10000}, half), {cx, solid.y - half});
        near(room.move({cx, solid.y + solid.height + half + 20}, {0, -10000}, half),
             {cx, solid.y + solid.height + half});
    }

    // Deslizar ao longo de uma face e sair pelo canto sem ficar preso.
    const Rectangle obstacle = room.getSolids()[4];
    for (int fps : {15, 30, 60, 144, 240})
    {
        Player sliding({obstacle.x - half, 300});
        for (int frame = 0; frame < fps; ++frame)
        {
            sliding.update({1, 1}, 0.5f / fps, room);
            assertFree(room, sliding.getPosition());
        }
        near(sliding.getPosition().x, obstacle.x - half);
        near(sliding.getPosition().y, 300 + 240 * 0.5f / std::sqrt(2.0f));
        for (int frame = 0; frame < fps; ++frame)
        {
            sliding.update({1, 1}, 1.5f / fps, room);
            assertFree(room, sliding.getPosition());
        }
        assert(sliding.getPosition().x > obstacle.x + half);

        Player corner(room.getSpawn());
        for (int frame = 0; frame < fps * 4; ++frame)
        {
            corner.update({-1, -1}, 1.0f / fps, room);
            assertFree(room, corner.getPosition());
        }
        near(corner.getPosition(), {wall + half, wall + half});
    }

    // Contato prolongado em faces fracionarias nao acumula deslocamento na parede.
    Room fractional(1437.3f, 987.7f);
    fractional.setObstacles({{300.13f, 400.0f, 97.23f, 98.001f}});
    const Rectangle face = fractional.getSolids()[4];
    const Vector2 contacts[] = {{face.x-half,face.y+40},
        {face.x+face.width+half,face.y+40}, {face.x+40,face.y-half},
        {face.x+40,face.y+face.height+half}, {200,987.7f-wall-half}};
    const Vector2 into[] = {{1,0},{-1,0},{0,1},{0,-1},{0,1}};
    for (int i=0;i<5;++i)
        for (int fps : {30,60,144,240})
        {
            Player pressed(contacts[i]);
            for (int frame=0;frame<fps*8;++frame)
                pressed.update(into[i],1.0f/fps,fractional);
            near(pressed.getPosition(),contacts[i]);
            const Vector2 beforeRelease=pressed.getPosition();
            pressed.update(Vector2Negate(into[i]),1.0f/fps,fractional);
            near(Vector2Distance(beforeRelease,pressed.getPosition()),240.0f/fps);
        }

    // Mira usa a posicao apos o movimento; mirar no proprio centro preserva a direcao.
    Player aiming(start);
    aiming.aimAt({200, 400});
    near(aiming.getDirection(), {0, 1});
    aiming.aimAt(start);
    near(aiming.getDirection(), {0, 1});
    near(aiming.getPosition(), start);
    aiming.update({0, -1}, 0.25f, room);
    aiming.aimAt({200, 400});
    near(aiming.getDirection(), {0, 1});

    // Zoom constante, acompanhamento e limites em salas de varias proporcoes.
    for (Vector2 size : {Vector2{1280, 800}, Vector2{800, 1200}, Vector2{2000, 600}, Vector2{500, 300}})
    {
        Room resized(size.x, size.y);
        const Vector2 center = {size.x / 2, size.y / 2};
        const Camera2D camera = resized.getCamera(960, 640, center);
        near(camera.zoom, 1.0f);
        near(camera.target, center);
        near(GetWorldToScreen2D(center, camera), camera.offset);
        const Vector2 worldPoint = {400, 300};
        near(GetScreenToWorld2D(GetWorldToScreen2D(worldPoint, camera), camera), worldPoint);
        for (Vector2 focus : {Vector2{0, 0}, size})
        {
            const Camera2D edge = resized.getCamera(960, 640, focus);
            const float halfWidth = 960 / (2 * edge.zoom);
            const float halfHeight = (640 - Config::hudHeight - Config::footerHeight) / (2 * edge.zoom);
            if (size.x >= halfWidth * 2)
            {
                assert(edge.target.x >= halfWidth - 0.01f);
                assert(edge.target.x <= size.x - halfWidth + 0.01f);
            }
            else near(edge.target.x, center.x);
            if (size.y >= halfHeight * 2)
            {
                assert(edge.target.y >= halfHeight - 0.01f);
                assert(edge.target.y <= size.y - halfHeight + 0.01f);
            }
            else near(edge.target.y, center.y);
        }
        assertFree(resized, resized.getSpawn());
    }
    const Camera2D before = room.getCamera(960, 640, {500, 300});
    const Camera2D after = room.getCamera(960, 640, {600, 400});
    near(after.target, {600, 400});
    near(after.target.x - before.target.x, 100);
    near(after.target.y - before.target.y, 100);
    Player movingAim({600, 400});
    movingAim.aimAt(GetScreenToWorld2D({after.offset.x + 100, after.offset.y}, after));
    near(movingAim.getDirection(), {1, 0});
    // Janela maior escala o jogo sem distorcer a mira ou invadir o HUD/rodape.
    Room large(2400,1800);
    for (Vector2 viewport : {Vector2{960,640},Vector2{1280,720},Vector2{1920,1080},Vector2{1280,1024}})
    {
        const float scale = Config::windowScale(viewport.x,viewport.y);
        const Camera2D view = large.getCamera(viewport.x,viewport.y,{1200,900});
        near(view.zoom,scale);
        near(view.offset.x,viewport.x/2);
        const float available = viewport.y-(Config::hudHeight+Config::footerHeight)*scale;
        near(view.offset.y,Config::hudHeight*scale+available/2);
        const Vector2 point{1300,800};
        near(GetScreenToWorld2D(GetWorldToScreen2D(point,view),view),point);
        const Vector2 top = GetScreenToWorld2D({view.offset.x,Config::hudHeight*scale},view);
        const Vector2 bottom = GetScreenToWorld2D({view.offset.x,viewport.y-Config::footerHeight*scale},view);
        near(bottom.y-top.y,available/view.zoom);
    }
    std::puts("OK: velocidade, diagonais, FPS, paredes, obstaculos, deslizamento, cantos, mira e camera.");
}
