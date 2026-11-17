#pragma once

#include "Room.h"
#include <random>

class Player;

class Dungeon
{
public:
    explicit Dungeon(unsigned int seed = std::random_device{}());
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

private:
    std::vector<Room> rooms;
    std::size_t currentIndex = 0;
    bool shortcutOpen = false;
    bool keyCollected = false;
    int discoveredCount = 0;
    float transitionRemaining = 0;
    bool doorArmed = true;
    void populate(unsigned int seed);
    void connect(int from, int to, bool vertical = false, bool locked = false);
};
