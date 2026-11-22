// Recorta as duas folhas originais 4x4, preservando o desenho e alinhando os pes.
#include "raylib.h"
#include <algorithm>
#include <filesystem>
#include <string>

bool extract(const char* file, const char* directory, bool healer)
{
    Image source = LoadImage(file);
    if (!IsImageValid(source)) return false;
    Color* pixels = LoadImageColors(source);
    constexpr int cell = 384;
    Image atlas = GenImageColor(cell*4,cell*4,BLANK);
    auto* atlasPixels = static_cast<Color*>(atlas.data);
    const auto root = std::filesystem::path(directory);
    std::filesystem::create_directories(root / "frames");
    const char* watchmanNames[] = {"idle","talk","point","farewell"};
    const char* healerNames[] = {"idle","talk","heal","rest"};
    bool success = true;
    for (int row=0;row<4;++row)
    {
        const int top=row*source.height/4, bottom=(row+1)*source.height/4;
        int baseline=top;
        // Ignora particulas nas laterais ao procurar a linha dos pes.
        for (int column=0;column<4;++column)
        {
            const int center=column*source.width/4+(healer ? 152 : 185);
            for (int y=top;y<bottom;++y)
                for (int x=center-40;x<center+40;++x)
                    if (pixels[y*source.width+x].a>=96) baseline=std::max(baseline,y);
        }
        for (int column=0;column<4;++column)
        {
            const int left=column*source.width/4, right=(column+1)*source.width/4;
            const int center=left+(healer ? 152 : 185);
            Image frame=GenImageColor(cell,cell,BLANK);
            auto* framePixels=static_cast<Color*>(frame.data);
            for (int y=top;y<bottom;++y)
                for (int x=left;x<right;++x)
                {
                    Color pixel=pixels[y*source.width+x];
                    if (pixel.a<96) continue;
                    pixel.a=255;
                    const int dx=192+x-center, dy=360+y-baseline;
                    if (dx<0 || dx>=cell || dy<0 || dy>=cell) { success=false;continue; }
                    framePixels[dy*cell+dx]=pixel;
                    atlasPixels[(row*cell+dy)*atlas.width+column*cell+dx]=pixel;
                }
            const std::string name=std::string(healer ? healerNames[row] : watchmanNames[row])+"-"+std::to_string(column)+".png";
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
    return extract("Guarda medieval em animação pixel art.png","resources/npcs/watchman",false) &&
        extract("Spritesheet do Curandeiro Medieval.png","resources/npcs/healer",true) ? 0 : 1;
}
