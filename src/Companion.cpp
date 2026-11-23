#include "Companion.h"

#include "Player.h"
#include "Room.h"
#include "raymath.h"
#include <algorithm>
#include <cmath>

void Companion::recruit(Vector2 start)
{
    if (active) return;
    active = true;
    enterRoom(start);
}

void Companion::enterRoom(Vector2 start)
{
    position = start;
    path.clear();
    waypoint = 0;
    pathCooldown = 0;
    strikeRemaining = 0;
}

void Companion::update(float deltaTime, const Player& player, Room& room)
{
    if (!active) return;
    animationTime += deltaTime;
    attackCooldown = std::max(0.0f,attackCooldown-deltaTime);
    pathCooldown -= deltaTime;
    strikeRemaining = std::max(0.0f,strikeRemaining-deltaTime);
    moving = Vector2Distance(position,player.getPosition()) > 72;
    if (moving)
    {
        Vector2 destination = player.getPosition();
        if (!room.lineClear(position,destination,12))
        {
            if (pathCooldown <= 0)
            {
                Vector2 goal = destination;
                if (!room.isWalkable(static_cast<int>(goal.x/Config::cellSize),static_cast<int>(goal.y/Config::cellSize)))
                {
                    float nearest = 1e20f;
                    for (int row=0; row<room.getRows(); ++row)
                        for (int column=0; column<room.getColumns(); ++column)
                        {
                            const Vector2 center{(column+0.5f)*Config::cellSize,(row+0.5f)*Config::cellSize};
                            const float distance = Vector2Distance(center,destination);
                            if (room.isWalkable(column,row) && distance < nearest && room.lineClear(center,destination,12))
                            { nearest = distance; goal = center; }
                        }
                }
                path = room.findPath(position,goal);
                waypoint = 0;
                pathCooldown = Config::pathInterval;
            }
            while (waypoint < path.size() && Vector2Distance(position,path[waypoint]) < 3) ++waypoint;
            if (waypoint < path.size()) destination = path[waypoint];
        }
        const Vector2 offset = Vector2Subtract(destination,position);
        position = room.move(position,Vector2Scale(Vector2Normalize(offset),
            std::min(220*deltaTime,Vector2Length(offset))),12);
    }
    if (attackCooldown > 0) return;
    Enemy* target = nullptr;
    float nearest = 180;
    for (Enemy& enemy : room.getEnemies())
    {
        const float distance = Vector2Distance(position,enemy.getPosition());
        if (enemy.isAlive() && distance < nearest && room.lineClear(position,enemy.getPosition(),2))
        {
            target = &enemy;
            nearest = distance;
        }
    }
    if (target)
    {
        target->takeDamage(8);
        strikeTarget = target->getPosition();
        strikeRemaining = 0.15f;
        attackCooldown = 0.8f;
    }
}

void Companion::draw(Texture2D texture) const
{
    if (!active) return;
    DrawEllipse(position.x,position.y+12,12,4,Fade(BLACK,0.35f));
    if (IsTextureValid(texture))
    {
        const int frame = static_cast<int>(animationTime/0.16f)%4;
        const float bounce = moving ? std::sin(animationTime*18)*1.5f : 0;
        DrawTexturePro(texture,{frame*384.0f,strikeRemaining > 0 ? 768.0f : 0.0f,384,384},
            {position.x-192*0.13f,position.y+12-360*0.13f+bounce,384*0.13f,384*0.13f},{},0,{150,240,215,255});
    }
    else DrawPoly(position,6,12,0,Config::accent);
    DrawText("Vigia",position.x-17,position.y-42,12,Config::accent);
    if (strikeRemaining > 0) DrawLineEx(position,strikeTarget,2,Fade(Config::accent,strikeRemaining/0.15f));
}
