#include "Room.h"
#include "Player.h"
#include "Geometry.h"
#include "Scenery.h"
#include <queue>

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
    permanentSolids = solids;
    rebuildNavigation();
}

void Room::rebuildNavigation()
{
    for (int row = 0; row < rows; ++row)
        for (int column = 0; column < columns; ++column)
        {
            const Vector2 center{(column + 0.5f) * Config::cellSize, (row + 0.5f) * Config::cellSize};
            const float half = Config::enemyHalfSize;
            const Rectangle area{center.x - half, center.y - half, half * 2, half * 2};
            bool free = center.x + half <= bounds.width && center.y + half <= bounds.height;
            for (std::size_t i = 0; i < solids.size(); ++i)
            {
                Rectangle obstacle = solids[i];
                // Folga nos obstaculos evita caminhos tangentes que prendem nos cantos por arredondamento.
                if (i >= 4) { obstacle.x -= 1; obstacle.y -= 1; obstacle.width += 2; obstacle.height += 2; }
                if (CheckCollisionRecs(area, obstacle)) free = false;
            }
            cells[row * columns + column].walkable = free;
        }
}

Vector2 Room::move(Vector2 position, Vector2 displacement, float halfSize) const
{
    // Resolve cada eixo por varredura ate a face mais proxima, sem atravessar
    // obstaculos mesmo quando o deslocamento do quadro supera sua espessura.
    // Compara o centro com as mesmas faces expandidas usadas para parar.
    // Somar/subtrair halfSize de novo pode arredondar para dentro da parede.
    float nextX = position.x + displacement.x;
    for (const Rectangle& solid : solids)
    {
        if (position.y <= solid.y - halfSize || position.y >= solid.y + solid.height + halfSize)
            continue;
        if (displacement.x > 0 && position.x <= solid.x - halfSize)
            nextX = std::min(nextX, solid.x - halfSize);
        else if (displacement.x < 0 && position.x >= solid.x + solid.width + halfSize)
            nextX = std::max(nextX, solid.x + solid.width + halfSize);
    }
    position.x = nextX;

    float nextY = position.y + displacement.y;
    for (const Rectangle& solid : solids)
    {
        if (position.x <= solid.x - halfSize || position.x >= solid.x + solid.width + halfSize)
            continue;
        if (displacement.y > 0 && position.y <= solid.y - halfSize)
            nextY = std::min(nextY, solid.y - halfSize);
        else if (displacement.y < 0 && position.y >= solid.y + solid.height + halfSize)
            nextY = std::max(nextY, solid.y + solid.height + halfSize);
    }
    position.y = nextY;
    return position;
}

Camera2D Room::getCamera(int screenWidth, int screenHeight, Vector2 focus) const
{
    const float scale = Config::windowScale(screenWidth,screenHeight);
    const float availableHeight = screenHeight - (Config::hudHeight + Config::footerHeight)*scale;
    Camera2D camera = {};
    camera.zoom = Config::cameraZoom*scale;
    camera.offset = {screenWidth / 2.0f,
                     Config::hudHeight*scale + availableHeight / 2};
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
    const Color tint = id == 8 ? Color{195,225,212,255} : WHITE;
    if (!Scenery::tile(Scenery::Floor,bounds,128,128,tint))
    {
        DrawRectangleRec(bounds, id == 8 ? Color{23,39,39,255} : Config::floor);
        for (float x = Config::cellSize; x < bounds.width; x += Config::cellSize)
            DrawLine(x, 0, x, bounds.height, Color{37,44,54,255});
        for (float y = Config::cellSize; y < bounds.height; y += Config::cellSize)
            DrawLine(0, y, bounds.width, y, Color{37,44,54,255});
    }
    for (std::size_t i = 0; i < permanentSolids.size(); ++i)
    {
        const Rectangle solid = permanentSolids[i];
        DrawRectangleRec({solid.x+4,solid.y+5,solid.width,solid.height},Color{0,0,0,75});
        const bool textured = i == 2 || i == 3
            ? Scenery::tile(Scenery::Pillar,solid,Config::wallThickness,96)
            : Scenery::tile(Scenery::Wall,solid,96,Config::wallThickness);
        if (!textured) DrawRectangleRec(solid,i < 4 ? Config::wall : Config::obstacle);
        DrawRectangleLinesEx(solid,1,Color{24,27,35,255});
        DrawLineEx({solid.x+1,solid.y+1},{solid.x+solid.width-1,solid.y+1},2,Color{155,158,172,180});
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

void Room::reveal(Vector2 position, Vector2 direction)
{
    direction = Vector2LengthSqr(direction) > 0 ? Vector2Normalize(direction) : Vector2{1,0};
    if (!visionDirty && Vector2DistanceSqr(position,lastViewPosition) < 16 &&
        Vector2DistanceSqr(direction,lastViewDirection) < 0.0001f) return;
    lastViewPosition = position;
    lastViewDirection = direction;
    visionDirty = false;
    const float cosine = std::cos(Config::viewAngle * DEG2RAD / 2);
    for (int row = 0; row < rows; ++row)
        for (int column = 0; column < columns; ++column)
        {
            Cell& cell = cells[row * columns + column];
            const Vector2 center{(column + 0.5f) * Config::cellSize, (row + 0.5f) * Config::cellSize};
            const Vector2 offset = Vector2Subtract(center,position);
            const float distance = Vector2Length(offset);
            const bool inField = distance <= Config::nearVisionRadius ||
                (distance <= Config::revealRadius && Vector2DotProduct(Vector2Normalize(offset),direction) >= cosine);
            cell.visible = inField;
            if (inField)
            {
                const Rectangle area{column*Config::cellSize,row*Config::cellSize,Config::cellSize,Config::cellSize};
                const float cellEntry = hitFraction(position,center,area);
                for (const Rectangle& solid : solids)
                    if (hitFraction(position,center,solid) + 0.0001f < cellEntry) { cell.visible = false; break; }
            }
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
            DrawRectangleRec(area, {0,0,0,cell.explored ? Config::exploredShadow : Config::unexploredShadow});
        }
}

void Room::refreshSolids()
{
    visionDirty = true;
    solids = permanentSolids;
    for (const Box& box : boxes)
        if (box.health > 0) solids.push_back(box.bounds);
    rebuildNavigation();
}

void Room::addBox(Rectangle area)
{
    boxes.push_back({area});
    refreshSolids();
}

void Room::damageBox(std::size_t index, float damage, unsigned int swing)
{
    Box& box = boxes.at(index);
    if (box.health <= 0 || (swing != 0 && box.lastSwing == swing)) return;
    box.lastSwing = swing;
    box.health = std::max(0.0f, box.health - damage);
    if (box.health == 0) refreshSolids();
}

bool Room::interactLever(Vector2 position)
{
    for (Lever& lever : levers)
    {
        if (Vector2Distance(position, lever.position) > Config::interactionRadius) continue;
        lever.activated = true;
        for (Spikes& trap : spikes)
            if (trap.group == lever.group) trap.disabled = true;
        return true;
    }
    return false;
}

void Room::updateHazards(float deltaTime, Player& player)
{
    for (Spikes& trap : spikes)
    {
        if (trap.disabled) continue;
        trap.timer += deltaTime;
        const float durations[] = {Config::trapRestDuration, Config::trapWarningDuration, Config::trapActiveDuration};
        while (trap.timer >= durations[static_cast<int>(trap.state)])
        {
            trap.timer -= durations[static_cast<int>(trap.state)];
            trap.state = static_cast<SpikeState>((static_cast<int>(trap.state) + 1) % 3);
        }
        trap.damageCooldown = std::max(0.0f, trap.damageCooldown - deltaTime);
        if (trap.state != SpikeState::Active || trap.damageCooldown > 0) continue;
        const Vector2 pos = player.getPosition();
        const float half = Config::playerHalfSize;
        if (CheckCollisionRecs({pos.x-half, pos.y-half, half*2, half*2}, trap.bounds))
            player.takeDamage(Config::trapDamage);
        for (Enemy& enemy : enemies)
            if (enemy.isAlive() && CheckCollisionRecs(enemy.getBounds(), trap.bounds)) enemy.takeDamage(Config::trapDamage);
        trap.damageCooldown = Config::trapDamageInterval;
    }
}

void Npc::update(float deltaTime, bool talking, int line)
{
    if (type == NpcType::None) return;
    healingAnimationRemaining = std::max(0.0f,healingAnimationRemaining-deltaTime);
    const int row = talking ? (type == NpcType::Watchman ? std::min(3,line+1) : 1)
        : healingAnimationRemaining > 0 ? (healingAnimationRemaining > 0.64f ? 2 : 3) : 0;
    animationTime = row == animationRow ? animationTime+deltaTime : 0;
    animationRow = row;
}

void Room::drawInteractions(Vector2 playerPosition, Texture2D npcTexture) const
{
    for (const Box& box : boxes)
    {
        if (box.health <= 0) continue;
        if (Scenery::draw(Scenery::Crate,box.bounds)) continue;
        DrawRectangleRec(box.bounds, BROWN);
        DrawRectangleLinesEx(box.bounds, 3, BEIGE);
        DrawLineEx({box.bounds.x, box.bounds.y},
            {box.bounds.x + box.bounds.width, box.bounds.y + box.bounds.height}, 2, BEIGE);
    }
    for (const Spikes& trap : spikes)
    {
        const Color color = trap.disabled ? DARKGRAY : trap.state == SpikeState::Active ? RED :
            trap.state == SpikeState::Warning ? GOLD : GRAY;
        DrawRectangleRec(trap.bounds, Fade(color, 0.2f));
        DrawRectangleLinesEx(trap.bounds, 2, color);
        for (float x = trap.bounds.x + 10; x < trap.bounds.x + trap.bounds.width; x += 24)
            DrawTriangle({x, trap.bounds.y + 22}, {x + 8, trap.bounds.y + 6}, {x - 8, trap.bounds.y + 6}, color);
    }
    for (const Lever& lever : levers)
    {
        DrawCircleV(lever.position, 10, lever.activated ? Config::accent : GOLD);
        DrawLineEx(lever.position, {lever.position.x + (lever.activated ? 12 : -12), lever.position.y-20}, 4, WHITE);
        if (Vector2Distance(playerPosition, lever.position) <= Config::interactionRadius)
            DrawText(lever.activated ? "Desativada" : "E: desativar espinhos", lever.position.x-50, lever.position.y-44, 14, GOLD);
    }
    if (npc.type != NpcType::None)
    {
        if (IsTextureValid(npcTexture))
        {
            constexpr float scale = 0.16f;
            DrawEllipse(npc.position.x,npc.position.y+14,16,5,Fade(BLACK,0.35f));
            DrawTexturePro(npcTexture,{npc.getAnimationFrame()*384.0f,npc.animationRow*384.0f,384,384},
                {npc.position.x-192*scale,npc.position.y+14-360*scale,384*scale,384*scale},{},0,WHITE);
        }
        else DrawPoly(npc.position, 6, 18, 30, npc.type == NpcType::Healer ? GREEN : SKYBLUE);
        DrawText(npc.type == NpcType::Healer ? "Curandeiro" : "Vigia", npc.position.x-36, npc.position.y-51, 16, WHITE);
        if (Vector2Distance(playerPosition, npc.position) <= Config::interactionRadius)
            DrawText("E: conversar", npc.position.x-42, npc.position.y+24, 14, GOLD);
    }
}

bool Room::freePosition(Vector2 position, float half) const
{
    if (position.x-half < 0 || position.y-half < 0 || position.x+half > bounds.width || position.y+half > bounds.height) return false;
    const Rectangle area{position.x-half, position.y-half, half*2, half*2};
    for (const Rectangle& solid : solids)
        if (CheckCollisionRecs(area, solid)) return false;
    return true;
}

bool Room::lineClear(Vector2 start, Vector2 end, float radius) const
{
    for (const Rectangle& solid : solids)
        if (hitFraction(start, end, solid, radius) <= 1) return false;
    return true;
}

std::vector<Vector2> Room::findPath(Vector2 start, Vector2 target) const
{
    const int sx = static_cast<int>(start.x / Config::cellSize), sy = static_cast<int>(start.y / Config::cellSize);
    const int tx = static_cast<int>(target.x / Config::cellSize), ty = static_cast<int>(target.y / Config::cellSize);
    if (sx < 0 || sy < 0 || sx >= columns || sy >= rows || !isWalkable(tx, ty)) return {};
    const int first = sy*columns+sx, goal = ty*columns+tx;
    std::vector<int> parent(cells.size(), -1);
    std::queue<int> pending;
    parent[first] = first;
    pending.push(first);
    while (!pending.empty() && parent[goal] == -1)
    {
        const int current = pending.front(); pending.pop();
        const int dx[] = {-1,1,0,0}, dy[] = {0,0,-1,1};
        for (int i = 0; i < 4; ++i)
        {
            const int x = current%columns+dx[i], y = current/columns+dy[i];
            if (!isWalkable(x,y)) continue;
            const int next = y*columns+x;
            if (parent[next] != -1) continue;
            parent[next] = current;
            pending.push(next);
        }
    }
    if (parent[goal] == -1) return {};
    std::vector<Vector2> path;
    for (int cell = goal; cell != first; cell = parent[cell])
        path.push_back({(cell%columns+0.5f)*Config::cellSize, (cell/columns+0.5f)*Config::cellSize});
    std::reverse(path.begin(), path.end());
    return path;
}

int EnemyShot::getSpriteRow() const
{
    return (static_cast<int>(std::lround(std::atan2(velocity.y,velocity.x)/(PI/4)))+10)%8;
}

void Room::updateEnemyShots(float deltaTime, Player& player)
{
    const Vector2 pos = player.getPosition();
    const float half = Config::playerHalfSize;
    const Rectangle target{pos.x-half, pos.y-half, half*2, half*2};
    for (EnemyShot& shot : enemyShots)
    {
        shot.age += deltaTime;
        const Vector2 end = Vector2Add(shot.position, Vector2Scale(shot.velocity, std::min(deltaTime, shot.remaining)));
        float nearest = hitFraction(shot.position, end, target, 4);
        bool hitPlayer = nearest <= 1;
        int boxIndex = -1;
        for (const Rectangle& solid : permanentSolids)
        {
            const float fraction = hitFraction(shot.position, end, solid, 4);
            if (fraction <= nearest) { nearest = fraction; hitPlayer = false; }
        }
        for (std::size_t i = 0; i < boxes.size(); ++i)
        {
            if (boxes[i].health <= 0) continue;
            const float fraction = hitFraction(shot.position, end, boxes[i].bounds, 4);
            if (fraction <= nearest) { nearest = fraction; hitPlayer = false; boxIndex = static_cast<int>(i); }
        }
        if (nearest <= 1)
        {
            if (hitPlayer) player.takeDamage(shot.damage);
            if (boxIndex >= 0) damageBox(boxIndex, shot.damage);
            shot.remaining = 0;
        }
        shot.position = Vector2Lerp(shot.position, end, std::min(1.0f, nearest));
        shot.remaining -= deltaTime;
    }
    std::erase_if(enemyShots, [](const EnemyShot& shot) { return shot.remaining <= 0; });
}

void Room::setObstacles(const std::vector<Rectangle>& obstacles)
{
    permanentSolids.resize(4);
    permanentSolids.insert(permanentSolids.end(), obstacles.begin(), obstacles.end());
    refreshSolids();
}

void Room::configureWaves(std::vector<std::vector<Enemy>> reinforcements, std::vector<Vector2> candidates)
{
    waves = std::move(reinforcements);
    waveCandidates = std::move(candidates);
    nextWave = 0;
    waveRemaining = Config::waveDelay;
}

bool Room::hasLivingEnemies() const
{
    return std::any_of(enemies.begin(),enemies.end(),
        [](const Enemy& enemy) { return enemy.isAlive(); });
}

bool Room::isWaveWaiting() const
{
    return hasPendingWaves() && !hasLivingEnemies();
}

void Room::updateWaves(float deltaTime, const Player& player)
{
    if (!isWaveWaiting()) return;
    waveRemaining = std::max(0.0f,waveRemaining-deltaTime);
    if (waveRemaining > 0) return;
    // Valida o grupo inteiro antes de inserir, preservando avisos e evitando surgimentos parciais.
    std::vector<Enemy> incoming;
    for (const Enemy& planned : waves[nextWave])
    {
        Vector2 selected{};
        float best = -1;
        for (Vector2 candidate : waveCandidates)
        {
            const float distance = Vector2Distance(candidate,player.getPosition());
            if (distance < Config::waveSpawnDistance || !freePosition(candidate,Config::enemyHalfSize+4)) continue;
            bool occupied = false;
            for (const Enemy& other : incoming)
                if (Vector2Distance(candidate,other.getPosition()) < 64) occupied = true;
            if (occupied) continue;
            // Mantem o ponto planejado quando for seguro; alternativas tambem sao anunciadas.
            const float preference = distance + (Vector2Distance(candidate,planned.getPosition()) < 1 ? 10000 : 0);
            if (preference > best) { selected = candidate; best = preference; }
        }
        if (best < 0) return;
        incoming.emplace_back(selected,planned.getType());
    }
    enemies = std::move(incoming);
    ++nextWave;
    waveRemaining = Config::waveDelay;
}

void Room::updateEnemyDeaths(float deltaTime)
{
    for (Enemy& enemy : dyingEnemies) enemy.updateDeathAnimation(deltaTime);
    std::erase_if(dyingEnemies,[](const Enemy& enemy) { return enemy.deathAnimationFinished(); });
}

void Room::drawWaveWarning() const
{
    if (!isWaveWaiting()) return;
    for (Vector2 candidate : waveCandidates)
    {
        if (!isVisible(candidate)) continue;
        const float radius = 20 + 5 * std::sin(waveRemaining*8);
        DrawCircleLines(candidate.x,candidate.y,radius,ORANGE);
        DrawLineEx({candidate.x-8,candidate.y},{candidate.x+8,candidate.y},2,ORANGE);
        DrawLineEx({candidate.x,candidate.y-8},{candidate.x,candidate.y+8},2,ORANGE);
    }
}
