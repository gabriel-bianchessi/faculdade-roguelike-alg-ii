#include "Room.h"

#include <algorithm>

Room::Room(float width, float height)
    : bounds{0, 0, width, height}, spawn{width * 0.12f, height * 0.2f}
{
    const float wall = Config::wallThickness;
    solids = {
        {0, 0, width, wall},
        {0, height - wall, width, wall},
        {0, wall, wall, height - wall * 2},
        {width - wall, wall, wall, height - wall * 2},
        {width * 0.32f, height * 0.2f, width * 0.05f, height * 0.35f},
        {width * 0.55f, height * 0.65f, width * 0.23f, height * 0.07f},
        {width * 0.7f, height * 0.22f, width * 0.1f, height * 0.14f}
    };
}

Vector2 Room::move(Vector2 position, Vector2 displacement, float halfSize) const
{
    // Resolve cada eixo por varredura ate a face mais proxima, sem atravessar
    // obstaculos mesmo quando o deslocamento do quadro supera sua espessura.
    float nextX = position.x + displacement.x;
    for (const Rectangle& solid : solids)
    {
        if (position.y + halfSize <= solid.y || position.y - halfSize >= solid.y + solid.height)
            continue;
        if (displacement.x > 0 && position.x + halfSize <= solid.x)
            nextX = std::min(nextX, solid.x - halfSize);
        else if (displacement.x < 0 && position.x - halfSize >= solid.x + solid.width)
            nextX = std::max(nextX, solid.x + solid.width + halfSize);
    }
    position.x = nextX;

    float nextY = position.y + displacement.y;
    for (const Rectangle& solid : solids)
    {
        if (position.x + halfSize <= solid.x || position.x - halfSize >= solid.x + solid.width)
            continue;
        if (displacement.y > 0 && position.y + halfSize <= solid.y)
            nextY = std::min(nextY, solid.y - halfSize);
        else if (displacement.y < 0 && position.y - halfSize >= solid.y + solid.height)
            nextY = std::max(nextY, solid.y + solid.height + halfSize);
    }
    position.y = nextY;
    return position;
}

Camera2D Room::getCamera(int screenWidth, int screenHeight, Vector2 focus) const
{
    const float availableHeight = screenHeight - Config::hudHeight - Config::footerHeight;
    Camera2D camera = {};
    camera.zoom = Config::cameraZoom;
    camera.offset = {screenWidth / 2.0f,
                     Config::hudHeight + availableHeight / 2};
    const float halfWidth = screenWidth / (2 * camera.zoom);
    const float halfHeight = availableHeight / (2 * camera.zoom);
    // Acompanha o jogador sem mostrar o exterior da sala. Salas menores que a
    // area visivel ficam centralizadas, mantendo o mesmo zoom.
    camera.target = {
        bounds.width <= halfWidth * 2 ? bounds.x + bounds.width / 2
            : std::clamp(focus.x, bounds.x + halfWidth, bounds.x + bounds.width - halfWidth),
        bounds.height <= halfHeight * 2 ? bounds.y + bounds.height / 2
            : std::clamp(focus.y, bounds.y + halfHeight, bounds.y + bounds.height - halfHeight)
    };
    return camera;
}

void Room::draw() const
{
    DrawRectangleRec(bounds, Config::floor);
    for (std::size_t i = 0; i < solids.size(); ++i)
    {
        DrawRectangleRec(solids[i], i < 4 ? Config::wall : Config::obstacle);
        DrawRectangleLinesEx(solids[i], 2, Config::muted);
    }
}
