#pragma once

#include "Room.h"
#include "Difficulty.h"
#include <random>

class Player;

class Dungeon
{
public:
    explicit Dungeon(unsigned int seed = std::random_device{}(), Difficulty difficulty = Difficulty::Normal);
    Room& currentRoom() { return rooms[currentIndex]; }
    const Room& currentRoom() const { return rooms[currentIndex]; }
    Room* findRoom(int id);
    bool update(float deltaTime, Player& player);
    void drawDetails() const;
    bool hasKey() const { return keyCollected; }
    int getDiscoveredCount() const { return discoveredCount; }
    void finishDialogue(Player& player);
    bool isShortcutOpen() const { return shortcutOpen; }
    bool nearLockedDoor(Vector2 position) const;
    bool interactSecret(Vector2 position);
    bool isSecretOpen() const { return secretOpen; }
    Vector2 getSecretPosition() const;

private:
    std::vector<Room> rooms;
    std::size_t currentIndex = 0;
    bool shortcutOpen = false;
    bool secretOpen = false;
    bool keyCollected = false;
    int discoveredCount = 0;
    float transitionRemaining = 0;
    bool doorArmed = true;
    void populate(unsigned int seed, Difficulty difficulty);
    void connect(int from, int to, bool vertical = false, bool locked = false);
};
