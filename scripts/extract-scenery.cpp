// Recortes da imagem original, sem gerar ou redesenhar a arte.
#include "raylib.h"
#include <filesystem>
#include <string>

int main()
{
    Image source = LoadImage("Câmara de Pedra com Quatro Portais.png");
    if (!IsImageValid(source)) return 1;
    std::filesystem::create_directories("resources/scenery");
    struct Piece { const char* name; Rectangle area; };
    const Piece pieces[] = {
        {"floor", {384,256,256,256}},
        {"wall", {380,76,216,32}},
        {"pillar", {318,12,52,166}},
        {"crate", {519,124,49,65}},
        {"portal-top", {674,0,184,174}},
        {"portal-bottom", {674,876,184,148}},
        {"portal-left", {12,418,104,188}},
        {"portal-right", {1420,418,104,188}}
    };
    bool success = true;
    for (const Piece& piece : pieces)
    {
        Image crop = ImageFromImage(source,piece.area);
        const std::string path = std::string("resources/scenery/")+piece.name+".png";
        success = ExportImage(crop,path.c_str()) && success;
        UnloadImage(crop);
    }
    UnloadImage(source);
    return success ? 0 : 1;
}
