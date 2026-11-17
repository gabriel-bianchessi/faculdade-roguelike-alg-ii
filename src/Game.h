#pragma once

#include "raylib.h"
#include "Player.h"
#include "Room.h"
#include "Combat.h"
#include "Dungeon.h"

enum class GameState
{
    Menu,
    Playing,
    Paused,
    Attributes,
    Dialogue,
    Victory,
    Defeat
};

class Game
{
public:
    void run();

private:
    struct Input
    {
        bool up = false;
        bool down = false;
        bool confirm = false;
        bool escape = false;
        bool click = false;
        bool attack = false;
        bool switchWeapon = false;
        bool dash = false;
        bool interact = false;
        bool attributes = false;
        Vector2 mouse = {};
        Vector2 movement = {};
    };

    GameState state = GameState::Menu;
    Input input;
    bool exitRequested = false;
    bool showingInstructions = false;
    int selectedOption = 0;
    float elapsedTime = 0.0f;
    int dialogueLine = 0;
    int helpPage = 0;
    Dungeon dungeon;
    Player player;
    Combat combat;
    Camera2D camera = {};
    Texture2D knightTexture = {};
    std::array<Texture2D,3> enemyTextures = {};

    void initialize();
    void readInput();
    void update(float deltaTime);
    void draw() const;
    void shutdown();
    void startNewGame();
    void changeState(GameState nextState);
    int readMenuChoice(int optionCount);
    Rectangle optionBounds(int index) const;
    void drawOptions(const char* const* options, int count) const;
    static void drawCentered(const char* text, int y, int fontSize, Color color);
    void drawWorld() const;
    void drawInstructions() const;
    void drawMainMenu() const;
    const char* dialogueText() const;
};
