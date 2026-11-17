#pragma once
#include "raymath.h"
#include <algorithm>
#include <cmath>

// Varredura: primeira fracao do segmento que toca a caixa.
inline float hitFraction(Vector2 start, Vector2 end, Rectangle box, float radius = 0)
{
    box.x -= radius; box.y -= radius;
    box.width += radius * 2; box.height += radius * 2;
    float enter = 0, leave = 1;
    const Vector2 delta = Vector2Subtract(end, start);
    const float origins[] = {start.x, start.y};
    const float directions[] = {delta.x, delta.y};
    const float minima[] = {box.x, box.y};
    const float maxima[] = {box.x + box.width, box.y + box.height};
    for (int axis = 0; axis < 2; ++axis)
    {
        if (std::abs(directions[axis]) < 0.00001f)
        {
            if (origins[axis] < minima[axis] || origins[axis] > maxima[axis]) return 2;
            continue;
        }
        float first = (minima[axis] - origins[axis]) / directions[axis];
        float last = (maxima[axis] - origins[axis]) / directions[axis];
        if (first > last) std::swap(first, last);
        enter = std::max(enter, first);
        leave = std::min(leave, last);
        if (enter > leave) return 2;
    }
    return enter;
}
