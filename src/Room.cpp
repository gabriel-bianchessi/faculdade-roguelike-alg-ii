#include "Room.h"

#include "raymath.h"

#include <algorithm>
#include <cmath>

Room::Room(float width, float height, int identifier)
    : id(identifier), columns(static_cast<int>(std::ceil(width / Config::cellSize))),
      rows(static_cast<int>(std::ceil(height / Config::cellSize))), cells(columns * rows),
      bounds{0, 0, width, height}, spawn{width * 0.12f, height * 0.2f}
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
    for (int row = 0; row < rows; ++row)
        for (int column = 0; column < columns; ++column)
        {
            const Vector2 center{(column + 0.5f) * Config::cellSize, (row + 0.5f) * Config::cellSize};
            const float half = Config::enemyHalfSize;
            const Rectangle area{center.x - half, center.y - half, half * 2, half * 2};
            bool free = center.x + half <= width && center.y + half <= height;
            for (const Rectangle& solid : solids)
                if (CheckCollisionRecs(area, solid)) free = false;
            cells[row * columns + column].walkable = free;
        }
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

bool Room::discover()
{
    if (discovered) return false;
    discovered = true;
    return true;
}

bool Room::isWalkable(int column, int row) const
{
    return column >= 0 && row >= 0 && column < columns && row < rows &&
           cells[row * columns + column].walkable;
}

bool Room::isVisible(Vector2 position) const
{
    const int column = static_cast<int>(std::floor(position.x / Config::cellSize));
    const int row = static_cast<int>(std::floor(position.y / Config::cellSize));
    return column >= 0 && row >= 0 && column < columns && row < rows &&
           cells[row * columns + column].visible;
}

void Room::reveal(Vector2 position)
{
    for (int row = 0; row < rows; ++row)
        for (int column = 0; column < columns; ++column)
        {
            Cell& cell = cells[row * columns + column];
            const Vector2 center{(column + 0.5f) * Config::cellSize, (row + 0.5f) * Config::cellSize};
            cell.visible = Vector2DistanceSqr(center, position) <= Config::revealRadius * Config::revealRadius;
            cell.explored = cell.explored || cell.visible;
        }
}

bool Room::collectKey(Vector2 position)
{
    if (!keyPresent || Vector2Distance(position, keyPosition) > Config::playerHalfSize + 12) return false;
    keyPresent = false;
    return true;
}

void Room::drawExploration() const
{
    for (int row = 0; row < rows; ++row)
        for (int column = 0; column < columns; ++column)
        {
            const Cell& cell = cells[row * columns + column];
            if (cell.visible) continue;
            const Rectangle area{column * Config::cellSize, row * Config::cellSize,
                std::min(Config::cellSize, bounds.width - column * Config::cellSize),
                std::min(Config::cellSize, bounds.height - row * Config::cellSize)};
            DrawRectangleRec(area, cell.explored ? Color{0, 0, 0, 180} : BLACK);
        }
}
