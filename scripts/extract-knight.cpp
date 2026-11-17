// Recorta a folha fornecida sem redesenhar os pixels. Executar na raiz do projeto.
#include "raylib.h"
#include <array>
#include <filesystem>
#include <string>

int main()
{
    Image source = LoadImage("sprites-knight.png");
    if (!IsImageValid(source)) return 1;
    Color* pixels = LoadImageColors(source);
    constexpr int width = 192, height = 128, anchorX = 80, anchorY = 116;
    const char* names[] = {"idle", "walk-down", "walk-left", "walk-right", "walk-up", "attack", "hurt", "death"};
    const int counts[] = {4,4,4,4,4,4,2,4};
    const int baseline[] = {115,229,329,426,531,643,760,872};
    const int centers[8][4] = {{290,505,724,943},{291,510,728,947},{286,504,720,938},
        {296,515,730,948},{287,504,721,941},{289,495,714,940},{289,484,0,0},{289,505,714,944}};
    // Areas excluem as legendas e a grade; ataque e morte precisam de recortes mais largos.
    const std::array<Rectangle,4> areas[] = {
        {{{241,9,99,108},{456,9,98,108},{675,9,98,108},{894,9,98,108}}},
        {{{243,126,97,104},{462,126,96,104},{681,126,95,104},{899,126,97,104}}},
        {{{242,239,89,91},{460,239,89,91},{676,239,88,91},{894,239,89,91}}},
        {{{252,335,89,92},{471,335,88,92},{686,335,88,92},{904,335,89,92}}},
        {{{243,432,89,100},{460,432,89,100},{677,432,89,100},{897,432,88,100}}},
        {{{239,540,100,104},{446,540,134,104},{665,540,161,104},{891,540,97,104}}},
        {{{239,654,100,107},{432,654,135,107},{0,0,0,0},{0,0,0,0}}},
        {{{240,768,100,105},{454,768,104,105},{661,768,133,105},{881,768,139,105}}}
    };
    const std::filesystem::path directory("resources/player");
    std::filesystem::create_directories(directory / "frames");
    Image atlas = GenImageColor(width*4,height*8,BLANK);
    auto* atlasPixels = static_cast<Color*>(atlas.data);
    bool success = true;
    for (int row=0; row<8; ++row)
        for (int column=0; column<counts[row]; ++column)
        {
            Image frame = GenImageColor(width,height,BLANK);
            auto* framePixels = static_cast<Color*>(frame.data);
            const Rectangle area = areas[row][column];
            for (int y=static_cast<int>(area.y); y<area.y+area.height; ++y)
                for (int x=static_cast<int>(area.x); x<area.x+area.width; ++x)
                {
                    Color pixel = pixels[y*source.width+x];
                    if (pixel.a < 96) continue; // Remove residuos quase transparentes da tabela.
                    pixel.a = 255;
                    const int dx = anchorX+x-centers[row][column], dy = anchorY+y-baseline[row];
                    if (dx<0 || dx>=width || dy<0 || dy>=height) continue;
                    framePixels[dy*width+dx] = pixel;
                    atlasPixels[(row*height+dy)*atlas.width+column*width+dx] = pixel;
                }
            const std::string name = (directory / "frames" / (std::string(names[row])+"-"+std::to_string(column)+".png")).string();
            success = ExportImage(frame,name.c_str()) && success;
            UnloadImage(frame);
        }
    success = ExportImage(atlas,(directory / "knight.png").string().c_str()) && success;
    UnloadImage(atlas);
    UnloadImageColors(pixels);
    UnloadImage(source);
    return success ? 0 : 1;
}
