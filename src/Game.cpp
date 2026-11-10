#include "Game.h"

#include "Config.h"

void Game::run()
{
    initialize();
    while (!exitRequested && !WindowShouldClose())
    {
        readInput();
        update(GetFrameTime());
        draw();
    }
    shutdown();
}

void Game::initialize()
{
    SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);
    InitWindow(Config::windowWidth, Config::windowHeight, Config::windowTitle);
    SetExitKey(KEY_NULL);
    SetTargetFPS(Config::targetFps);
}

void Game::readInput()
{
    input.up = IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W);
    input.down = IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S);
    input.confirm = IsKeyPressed(KEY_ENTER);
    input.escape = IsKeyPressed(KEY_ESCAPE);
    input.click = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
    input.mouse = GetMousePosition();
    input.mouseDelta = GetMouseDelta();
}

void Game::changeState(GameState nextState)
{
    state = nextState;
    selectedOption = 0;
    showingInstructions = false;
}

void Game::startNewGame()
{
    // Todo estado da partida deve ser recriado aqui conforme os sistemas surgirem.
    elapsedTime = 0.0f;
    changeState(GameState::Playing);
}

Rectangle Game::optionBounds(int index)
{
    return {Config::windowWidth / 2.0f - 170, 290.0f + index * 64, 340, 50};
}

int Game::readMenuChoice(int optionCount)
{
    if (input.up) selectedOption = (selectedOption + optionCount - 1) % optionCount;
    if (input.down) selectedOption = (selectedOption + 1) % optionCount;

    for (int i = 0; i < optionCount; ++i)
    {
        if (CheckCollisionPointRec(input.mouse, optionBounds(i)))
        {
            if (input.mouseDelta.x != 0 || input.mouseDelta.y != 0 || input.click)
                selectedOption = i;
            if (input.click) return i;
        }
    }
    return input.confirm ? selectedOption : -1;
}

void Game::update(float deltaTime)
{
    switch (state)
    {
        case GameState::Menu:
        {
            if (showingInstructions)
            {
                if (input.escape || input.confirm ||
                    (input.click && CheckCollisionPointRec(input.mouse, optionBounds(4))))
                    showingInstructions = false;
                break;
            }
            const int choice = readMenuChoice(3);
            if (choice == 0) startNewGame();
            else if (choice == 1) showingInstructions = true;
            else if (choice == 2) exitRequested = true;
            break;
        }
        case GameState::Playing:
            if (input.escape)
            {
                changeState(GameState::Paused);
                break;
            }
            elapsedTime += deltaTime;
            break;
        case GameState::Paused:
        {
            if (input.escape)
            {
                changeState(GameState::Playing);
                break;
            }
            const int choice = readMenuChoice(3);
            if (choice == 0) changeState(GameState::Playing);
            else if (choice == 1) startNewGame();
            else if (choice == 2) changeState(GameState::Menu);
            break;
        }
        case GameState::Attributes:
            // A distribuicao sera implementada junto com a progressao (etapa 6).
            if (input.escape) changeState(GameState::Playing);
            break;
        case GameState::Victory:
        case GameState::Defeat:
        {
            const int choice = readMenuChoice(2);
            if (choice == 0) startNewGame();
            else if (choice == 1 || input.escape) changeState(GameState::Menu);
            break;
        }
    }
}

void Game::drawCentered(const char* text, int y, int fontSize, Color color)
{
    DrawText(text, (Config::windowWidth - MeasureText(text, fontSize)) / 2,
             y, fontSize, color);
}

void Game::drawOptions(const char* const* options, int count) const
{
    for (int i = 0; i < count; ++i)
    {
        const Rectangle bounds = optionBounds(i);
        DrawRectangleRec(bounds, Config::panel);
        if (i == selectedOption) DrawRectangleLinesEx(bounds, 2, Config::accent);
        drawCentered(options[i], static_cast<int>(bounds.y) + 14, 22,
                     i == selectedOption ? Config::accent : Config::text);
    }
}

void Game::drawWorld() const
{
    DrawRectangle(80, 110, Config::windowWidth - 160, Config::windowHeight - 200,
                  Config::panel);
    drawCentered("Fortaleza do guardiao", 145, 28, Config::text);
    drawCentered("A exploracao e o combate serao adicionados nas proximas etapas.",
                 310, 20, Config::muted);
    DrawText(TextFormat("Tempo de partida: %.1f s", elapsedTime), 24, 24, 22, Config::text);
    DrawText("Esc: pausar", 24, Config::windowHeight - 40, 20, Config::muted);
}

void Game::drawInstructions() const
{
    drawCentered("Instrucoes", 70, 36, Config::accent);
    drawCentered("Entre na fortaleza e derrote seu guardiao corrompido.", 140, 22, Config::text);
    drawCentered("Menu: setas ou W/S e Enter; ou clique nas opcoes.", 200, 20, Config::text);
    drawCentered("Esc: pausar e retomar a partida.", 240, 20, Config::text);
    drawCentered("Controles previstos para as proximas etapas:", 310, 20, Config::muted);
    drawCentered("WASD: mover | Mouse: mirar | Botao esquerdo: atacar", 355, 20, Config::text);
    drawCentered("Q: trocar arma | Espaco: dash | E: interagir", 395, 20, Config::text);
    const Rectangle back = optionBounds(4);
    DrawRectangleRec(back, Config::panel);
    DrawRectangleLinesEx(back, 2, Config::accent);
    drawCentered("Voltar (Enter / Esc)", static_cast<int>(back.y) + 14, 22, Config::accent);
}

void Game::draw() const
{
    BeginDrawing();
    ClearBackground(Config::background);
    if (state == GameState::Menu)
    {
        if (showingInstructions) drawInstructions();
        else
        {
            drawCentered("ROGUE LIKE", 110, 48, Config::accent);
            drawCentered("A fortaleza do guardiao", 180, 24, Config::muted);
            const char* options[] = {"Iniciar partida", "Instrucoes", "Sair"};
            drawOptions(options, 3);
            drawCentered("Setas / W e S + Enter ou mouse", 550, 20, Config::muted);
        }
    }
    else
    {
        drawWorld();
        if (state != GameState::Playing)
        {
            DrawRectangle(0, 0, Config::windowWidth, Config::windowHeight, Config::overlay);
            if (state == GameState::Paused)
            {
                drawCentered("Pausa", 180, 40, Config::accent);
                const char* options[] = {"Retomar", "Reiniciar partida", "Voltar ao menu"};
                drawOptions(options, 3);
            }
            else if (state == GameState::Attributes)
            {
                drawCentered("Distribuicao de atributos", 180, 36, Config::accent);
                drawCentered("Esc: voltar", 310, 22, Config::text);
            }
            else
            {
                drawCentered(state == GameState::Victory ? "Vitoria" : "Derrota",
                             180, 40, Config::accent);
                const char* options[] = {"Reiniciar partida", "Voltar ao menu"};
                drawOptions(options, 2);
            }
        }
    }
    EndDrawing();
}

void Game::shutdown()
{
    CloseWindow();
}
