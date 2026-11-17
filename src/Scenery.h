#pragma once

#include "raylib.h"
#include <algorithm>
#include <array>
#include <string>

// Um unico conjunto compartilhado pelas salas; a janela controla a vida das texturas.
namespace Scenery
{
    enum Piece { Floor, Wall, Pillar, Crate, PortalTop, PortalBottom, PortalLeft, PortalRight, Count };
    inline std::array<Texture2D,Count> textures{};

    inline void load()
    {
        const char* names[] = {"floor", "wall", "pillar", "crate", "portal-top",
            "portal-bottom", "portal-left", "portal-right"};
        const std::string app = GetApplicationDirectory();
        for (int i = 0; i < Count; ++i)
        {
            if (IsTextureValid(textures[i])) continue;
            const std::string file = std::string("resources/scenery/")+names[i]+".png";
            const std::string paths[] = {file,app+file,app+"../"+file,app+"../../"+file};
            for (const std::string& path : paths)
                if (FileExists(path.c_str()))
                {
                    textures[i] = LoadTexture(path.c_str());
                    if (IsTextureValid(textures[i])) SetTextureFilter(textures[i],TEXTURE_FILTER_POINT);
                    break;
                }
        }
    }

    inline void unload()
    {
        for (Texture2D& texture : textures)
        {
            if (IsTextureValid(texture)) UnloadTexture(texture);
            texture = {};
        }
    }

    inline bool draw(Piece piece, Rectangle area, Color tint = WHITE)
    {
        const Texture2D texture = textures[piece];
        if (!IsTextureValid(texture)) return false;
        DrawTexturePro(texture,{0,0,static_cast<float>(texture.width),static_cast<float>(texture.height)},
            area,{0,0},0,tint);
        return true;
    }

    inline bool tile(Piece piece, Rectangle area, float width, float height, Color tint = WHITE)
    {
        const Texture2D texture = textures[piece];
        if (!IsTextureValid(texture)) return false;
        for (int row = 0; row * height < area.height; ++row)
            for (int column = 0; column * width < area.width; ++column)
            {
                const float w = std::min(width,area.width-column*width);
                const float h = std::min(height,area.height-row*height);
                // Espelhamento alternado une as bordas do recorte de piso.
                const bool flipX = piece == Floor && column % 2;
                const bool flipY = piece == Floor && row % 2;
                const float sw = texture.width*w/width, sh = texture.height*h/height;
                const Rectangle source{flipX ? texture.width-sw : 0,flipY ? texture.height-sh : 0,
                    flipX ? -sw : sw,flipY ? -sh : sh};
                DrawTexturePro(texture,source,{area.x+column*width,area.y+row*height,w,h},{0,0},0,tint);
            }
        return true;
    }
}
