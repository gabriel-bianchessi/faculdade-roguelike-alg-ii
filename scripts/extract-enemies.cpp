// Separa as tres folhas fornecidas. Executar na raiz, sem contexto grafico.
#include "raylib.h"
#include <algorithm>
#include <filesystem>
#include <string>

int main()
{
    SetTraceLogLevel(LOG_WARNING);
    const char* sources[] = {"Spritesheet de Cavaleiro Corrompido.png",
        "Spritesheet do Besteiro Corrompido.png","Spritesheet do Patrulheiro Sombrio.png"};
    const char* types[] = {"chaser","shooter","patrol"};
    const char* animations[] = {"idle","walk-down","walk-left","walk-right","walk-up","attack","hurt","death"};
    constexpr int cell=256, anchorX=128, anchorY=236;
    for (int type=0; type<3; ++type)
    {
        Image source=LoadImage(sources[type]);
        if (!IsImageValid(source)) return 1;
        Color* pixels=LoadImageColors(source);
        Image atlas=GenImageColor(cell*4,cell*8,BLANK);
        auto* output=static_cast<Color*>(atlas.data);
        const auto directory=std::filesystem::path("resources/enemies") / types[type];
        std::filesystem::create_directories(directory / "frames");
        bool success=true;
        for (int row=0; row<8; ++row)
        {
            const int count=row==6 ? 2 : 4;
            const int top=row*source.height/8, bottom=(row+1)*source.height/8;
            int baseline=top;
            for (int y=top; y<bottom; ++y)
                for (int x=0; x<count*source.width/4; ++x)
                    if (pixels[y*source.width+x].a >= 96) baseline=std::max(baseline,y);
            for (int column=0; column<count; ++column)
            {
                const int left=column*source.width/4, right=(column+1)*source.width/4;
                const int center=(left+right)/2;
                Image frame=GenImageColor(cell,cell,BLANK);
                auto* destination=static_cast<Color*>(frame.data);
                for (int y=top; y<bottom; ++y)
                    for (int x=left; x<right; ++x)
                    {
                        // O efeito do disparo anterior invade a margem da ultima celula.
                        if (type==1 && row==5 && column==3 && x<690) continue;
                        Color pixel=pixels[y*source.width+x];
                        if (pixel.a<96) continue;
                        pixel.a=255;
                        const int dx=anchorX+x-center, dy=anchorY+y-baseline;
                        if (dx<0 || dx>=cell || dy<0 || dy>=cell) continue;
                        destination[dy*cell+dx]=pixel;
                        output[(row*cell+dy)*atlas.width+column*cell+dx]=pixel;
                    }
                const auto path=directory / "frames" / (std::string(animations[row])+"-"+std::to_string(column)+".png");
                success=ExportImage(frame,path.string().c_str()) && success;
                UnloadImage(frame);
            }
        }
        success=ExportImage(atlas,(directory / "atlas.png").string().c_str()) && success;
        UnloadImage(atlas);
        UnloadImageColors(pixels);
        UnloadImage(source);
        if (!success) return 1;
    }
}
