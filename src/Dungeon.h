#pragma once

#include "Room.h"

class Player;

class Dungeon
{
public:
    Dungeon();
    Room& currentRoom() { return rooms[currentIndex]; }
    const Room& currentRoom() const { return rooms[currentIndex]; }
    Room* findRoom(int id);
    bool update(float deltaTime, Player& player);
    void drawDetails() const;
    bool hasKey() const { return keyCollected; }
    int getDiscoveredCount() const { return discoveredCount; }
    bool nearLockedDoor(Vector2 position) const;

private:
    std::vector<Room> rooms;
    std::size_t currentIndex = 0;
    bool keyCollected = false;
    int discoveredCount = 0;
    float transitionRemaining = 0;
    bool doorArmed = true;
    void connect(int from, int to, bool vertical = false, bool locked = false);
};
