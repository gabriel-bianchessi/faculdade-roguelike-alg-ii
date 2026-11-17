#pragma once

#include "Config.h"
#include "Enemy.h"
#include "Item.h"

#include <vector>

struct Connection
{
    int from;
    int to;
    Rectangle door;
    Vector2 arrival;
    bool locked = false;
};

struct EnemyShot
{
    Vector2 position;
    Vector2 velocity;
    int damage = 10;
    float remaining = Config::hostileShotLifetime;
};

struct Box
{
    Rectangle bounds;
    float health = Config::boxHealth;
    unsigned int lastSwing = 0;
};

enum class SpikeState { Resting, Warning, Active };
struct Spikes
{
    Rectangle bounds;
    int group = 0;
    float timer = 0;
    float damageCooldown = 0;
    bool disabled = false;
    SpikeState state = SpikeState::Resting;
};
struct Lever
{
    Vector2 position;
    int group = 0;
    bool activated = false;
};
enum class NpcType { None, Watchman, Healer };
struct Npc
{
    NpcType type = NpcType::None;
    Vector2 position = {};
    bool talked = false;
    bool healingUsed = false;
};

struct Cell
{
    bool walkable = false;
    bool explored = false;
    bool visible = false;
};

class Room
{
public:
    explicit Room(float width = Config::roomWidth, float height = Config::roomHeight, int identifier = 1);
    Rectangle getBounds() const { return bounds; }
    Vector2 getSpawn() const { return spawn; }
    const std::vector<Rectangle>& getSolids() const { return solids; }
    Vector2 move(Vector2 position, Vector2 displacement, float halfSize) const;
    Camera2D getCamera(int screenWidth, int screenHeight, Vector2 focus) const;
    void draw() const;
    void drawExploration() const;
    void drawInteractions(Vector2 playerPosition) const;
    void addBox(Rectangle area);
    void setObstacles(const std::vector<Rectangle>& obstacles);
    void damageBox(std::size_t index, float damage, unsigned int swing = 0);
    const std::vector<Box>& getBoxes() const { return boxes; }
    void rebuildNavigation();
    std::vector<Spikes>& getSpikes() { return spikes; }
    std::vector<Lever>& getLevers() { return levers; }
    Npc& getNpc() { return npc; }
    const Npc& getNpc() const { return npc; }
    bool interactLever(Vector2 position);
    std::vector<Vector2> findPath(Vector2 start, Vector2 target) const;
    bool lineClear(Vector2 start, Vector2 end, float radius = 0) const;
    bool freePosition(Vector2 position, float halfSize) const;
    std::vector<EnemyShot>& getEnemyShots() { return enemyShots; }
    const std::vector<EnemyShot>& getEnemyShots() const { return enemyShots; }
    void updateEnemyShots(float deltaTime, Player& player);
    void updateHazards(float deltaTime, Player& player);
    void reveal(Vector2 position, Vector2 direction = {1,0});
    void configureWaves(std::vector<std::vector<Enemy>> reinforcements, std::vector<Vector2> candidates);
    void updateWaves(float deltaTime, const Player& player);
    void drawWaveWarning() const;
    int getWaveNumber() const { return waves.empty() ? 0 : static_cast<int>(nextWave)+1; }
    int getWaveCount() const { return waves.empty() ? 0 : static_cast<int>(waves.size())+1; }
    bool hasPendingWaves() const { return nextWave < waves.size(); }
    bool hasLivingEnemies() const;
    bool isWaveWaiting() const;
    float getWaveRemaining() const { return waveRemaining; }
    bool discover();
    int getId() const { return id; }
    bool isDiscovered() const { return discovered; }
    int getColumns() const { return columns; }
    int getRows() const { return rows; }
    const Cell& getCell(int column, int row) const { return cells.at(row * columns + column); }
    bool isWalkable(int column, int row) const;
    bool isVisible(Vector2 position) const;
    std::vector<Enemy>& getEnemies() { return enemies; }
    const std::vector<Enemy>& getEnemies() const { return enemies; }
    std::vector<Connection>& getConnections() { return connections; }
    const std::vector<Connection>& getConnections() const { return connections; }
    std::vector<Item>& getItems() { return items; }
    const std::vector<Item>& getItems() const { return items; }
    void placeKey(Vector2 position) { keyPosition = position; keyPresent = true; }
    bool collectKey(Vector2 position);
    bool hasKey() const { return keyPresent; }
    Vector2 getKeyPosition() const { return keyPosition; }


private:
    std::vector<std::vector<Enemy>> waves;
    std::vector<Vector2> waveCandidates;
    std::size_t nextWave = 0;
    float waveRemaining = Config::waveDelay;
    bool visionDirty = true;
    Vector2 lastViewPosition = {};
    Vector2 lastViewDirection = {1,0};
    std::vector<EnemyShot> enemyShots;
    std::vector<Box> boxes;
    std::vector<Spikes> spikes;
    std::vector<Lever> levers;
    Npc npc;
    std::vector<Rectangle> permanentSolids;
    void refreshSolids();
    int id;
    bool discovered = false;
    int columns;
    int rows;
    std::vector<Cell> cells;
    std::vector<Enemy> enemies;
    std::vector<Item> items;
    std::vector<Connection> connections;
    bool keyPresent = false;
    Vector2 keyPosition = {};
    Rectangle bounds;
    Vector2 spawn;
    std::vector<Rectangle> solids;
};
