#include "WeaponVisual.h"
#include "raymath.h"
#include <initializer_list>
#include <vector>

void drawWeapon(WeaponType type, Vector2 origin, Vector2 direction, float scale)
{
    const Color outline{18,21,24,255}, steel{161,178,181,255}, edge{224,229,215,255};
    const Color darkSteel{62,76,81,255}, brass{179,137,70,255}, leather{105,65,44,255};
    const Vector2 forward = Vector2Normalize(direction);
    Vector2 side{-forward.y,forward.x};
    if (forward.x < 0) side = Vector2Negate(side);
    auto point = [&](Vector2 local) {
        return Vector2Add(origin,Vector2Scale(Vector2Add(Vector2Scale(forward,local.x),Vector2Scale(side,local.y)),scale));
    };
    auto line = [&](Vector2 a, Vector2 b, float width, Color color) { DrawLineEx(point(a),point(b),width*scale,color); };
    auto polygon = [&](std::initializer_list<Vector2> vertices, Color color) {
        std::vector<Vector2> points;
        for (Vector2 vertex : vertices) points.push_back(point(vertex));
        for (std::size_t i=1; i+1<points.size(); ++i)
        {
            Vector2 a=points[0], b=points[i], c=points[i+1];
            if ((b.x-a.x)*(c.y-a.y)-(b.y-a.y)*(c.x-a.x)>0) std::swap(b,c);
            DrawTriangle(a,b,c,color);
        }
        for (std::size_t i=0; i<points.size(); ++i)
            DrawLineEx(points[i],points[(i+1)%points.size()],1.2f*scale,outline);
    };
    auto block = [&](float x,float y,float w,float h,Color color) {
        polygon({{x,y},{x+w,y},{x+w,y+h},{x,y+h}},color);
    };
    if (type == WeaponType::Sword)
    {
        polygon({{4,-5},{34,-4},{45,0},{34,4},{4,5}},steel);
        polygon({{5,0},{42,0},{33,-3},{5,-4}},edge);
        block(-9,-3,13,6,leather);
        for (int x=-7; x<3; x+=3) line({static_cast<float>(x),-2},{static_cast<float>(x+1),2},1,brass);
        polygon({{1,-10},{5,-9},{7,-4},{7,4},{5,9},{1,10}},brass);
        DrawCircleV(point({-11,0}),4*scale,brass);
    }
    else if (type == WeaponType::Spear)
    {
        line({-22,0},{48,0},6,outline);
        line({-21,0},{48,0},4,leather);
        line({-18,-1},{43,-1},1,brass);
        block(36,-4,9,8,brass);
        polygon({{44,-5},{64,0},{44,5},{48,0}},steel);
        line({47,0},{61,0},1,edge);
        for (int x=-5; x<15; x+=4) line({static_cast<float>(x),-2},{static_cast<float>(x+1),2},2,darkSteel);
    }
    else if (type == WeaponType::Pistol)
    {
        polygon({{-5,2},{5,2},{1,18},{-8,16}},leather);
        block(1,3,10,8,darkSteel);
        block(4,4,5,4,outline);
        polygon({{-5,-6},{23,-6},{27,-3},{27,3},{-5,5}},darkSteel);
        block(-3,-5,27,4,steel);
        line({-1,-6},{23,-6},1,edge);
        block(24,-3,4,6,outline);
        block(18,-8,3,2,brass);
        for (int x=-2; x<5; x+=3) line({static_cast<float>(x),-4},{static_cast<float>(x),0},1,outline);
    }
    else
    {
        polygon({{-17,-5},{-4,-4},{2,0},{-4,5},{-18,8},{-23,5}},leather);
        block(-5,-6,19,12,darkSteel);
        block(8,-5,35,4,steel);
        block(8,1,35,4,steel);
        line({12,-4},{41,-4},1,edge);
        block(14,-1,28,2,outline);
        block(9,-7,14,14,leather);
        for (int x=12; x<22; x+=3) line({static_cast<float>(x),-5},{static_cast<float>(x),5},1,brass);
        block(41,-6,3,12,outline);
        block(-1,5,9,7,brass);
        block(1,6,5,3,outline);
    }
}
