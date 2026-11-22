// Recorta as folhas fornecidas do chefe e dos disparos; executar na raiz.
#include "raylib.h"
#include <algorithm>
#include <filesystem>
#include <string>

bool extract(const char* file, const char* directory, bool boss)
{
    Image source=LoadImage(file);
    if (!IsImageValid(source)) return false;
    Color* pixels=LoadImageColors(source);
    const int bossRows[]={0,136,270,390,508,634,762,879,1001,1139,1265,1398,1536};
    const int shotRows[]={0,160,285,390,510,650,775,880,1024};
    const int shotColumns[]={410,590,770,950,1130};
    const int bodyCenters[]={143,395,646,897};
    const char* bossNames[]={"idle","walk-down","walk-left","walk-right","walk-up","charge-warning",
        "charge","fan-warning","fan-attack","recovery","hurt","death"};
    const char* shotNames[]={"north","north-east","east","south-east","south","south-west","west","north-west"};
    const int rows=boss ? 12 : 8;
    constexpr int cell=256;
    const auto root=std::filesystem::path(directory);
    std::filesystem::create_directories(root / "frames");
    Image atlas=GenImageColor(cell*4,cell*rows,BLANK);
    auto* atlasPixels=static_cast<Color*>(atlas.data);
    bool success=true;
    for (int row=0;row<rows;++row)
    {
        const int top=boss ? bossRows[row] : shotRows[row];
        const int bottom=boss ? bossRows[row+1] : shotRows[row+1];
        int baseline=top;
        if (boss)
            for (int y=top;y<bottom;++y)
                for (int x=0;x<source.width;++x)
                    if (pixels[y*source.width+x].a>=96) baseline=std::max(baseline,y);
        for (int column=0;column<4;++column)
        {
            const int left=boss ? column*256 : shotColumns[column];
            const int right=boss ? (column+1)*256 : shotColumns[column+1];
            int centerX=boss ? bodyCenters[column] : (left+right)/2;
            int centerY=boss ? baseline : (top+bottom)/2;
            if (!boss)
            {
                int sumX=0,sumY=0,count=0;
                for (int y=top;y<bottom;++y)
                    for (int x=left;x<right;++x)
                    {
                        const Color pixel=pixels[y*source.width+x];
                        if (pixel.a>=96 && pixel.r>210 && pixel.g>190 && pixel.b>210)
                        { sumX+=x;sumY+=y;++count; }
                    }
                if (count>0) { centerX=sumX/count;centerY=sumY/count; }
            }
            Image frame=GenImageColor(cell,cell,BLANK);
            auto* framePixels=static_cast<Color*>(frame.data);
            for (int y=top;y<bottom;++y)
                for (int x=left;x<right;++x)
                {
                    Color pixel=pixels[y*source.width+x];
                    if (pixel.a<96) continue;
                    pixel.a=255;
                    const int dx=128+x-centerX,dy=(boss ? 236 : 128)+y-centerY;
                    if (dx<0 || dx>=cell || dy<0 || dy>=cell) continue;
                    framePixels[dy*cell+dx]=pixel;
                    atlasPixels[(row*cell+dy)*atlas.width+column*cell+dx]=pixel;
                }
            const auto name=std::string(boss ? bossNames[row] : shotNames[row])+"-"+std::to_string(column)+".png";
            success=ExportImage(frame,(root / "frames" / name).string().c_str()) && success;
            UnloadImage(frame);
        }
    }
    success=ExportImage(atlas,(root / "atlas.png").string().c_str()) && success;
    UnloadImage(atlas);UnloadImageColors(pixels);UnloadImage(source);
    return success;
}

int main()
{
    SetTraceLogLevel(LOG_WARNING);
    return extract("Spritesheet do Cavaleiro Sombrio Corrompido.png","resources/enemies/boss",true) &&
        extract("Spritesheet de Projéteis Mágicos Roxos.png","resources/projectiles/hostile",false) ? 0 : 1;
}
