#pragma once

#include "raylib.h"
#include "Player.h"
#include "Room.h"
#include "Combat.h"

enum class GameState
{
    Menu,
    Playing,
    Paused,
    Attributes,
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
        Vector2 mouse = {};
        Vector2 mouseDelta = {};
        Vector2 movement = {};
    };

    GameState state = GameState::Menu;
    Input input;
    bool exitRequested = false;
    bool showingInstructions = false;
    int selectedOption = 0;
    float elapsedTime = 0.0f;
    Room room;
    Player player;
    Combat combat;
    Camera2D camera = {};

    void initialize();
    void readInput();
    void update(float deltaTime);
    void draw() const;
    void shutdown();
    void startNewGame();
    void changeState(GameState nextState);
    int readMenuChoice(int optionCount);
    static Rectangle optionBounds(int index);
    void drawOptions(const char* const* options, int count) const;
    static void drawCentered(const char* text, int y, int fontSize, Color color);
    void drawWorld() const;
    void drawInstructions() const;
};
