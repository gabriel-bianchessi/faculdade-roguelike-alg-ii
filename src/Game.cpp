#include "Game.h"

#include "Config.h"
#include "Scenery.h"
#include "WeaponVisual.h"

#include <algorithm>
#include <string>
#include "raymath.h"

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
    SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI | FLAG_WINDOW_RESIZABLE);
    InitWindow(Config::windowWidth, Config::windowHeight, Config::windowTitle);
    SetWindowMinSize(Config::windowWidth,Config::windowHeight);
    MaximizeWindow();
    const std::string appDirectory = GetApplicationDirectory();
    const auto loadSprite = [&appDirectory](const std::string& resource) {
        const std::string paths[] = {resource,appDirectory+resource,appDirectory+"../"+resource,
            appDirectory+"../../"+resource};
        for (const std::string& path : paths)
            if (FileExists(path.c_str()))
            {
                Texture2D texture = LoadTexture(path.c_str());
                if (IsTextureValid(texture)) SetTextureFilter(texture,TEXTURE_FILTER_POINT);
                return texture;
            }
        TraceLog(LOG_WARNING,"Sprite nao encontrado: %s",resource.c_str());
        return Texture2D{};
    };
    knightTexture = loadSprite("resources/player/knight.png");
    enemyTextures[0] = loadSprite("resources/enemies/chaser/atlas.png");
    enemyTextures[1] = loadSprite("resources/enemies/shooter/atlas.png");
    enemyTextures[2] = loadSprite("resources/enemies/patrol/atlas.png");
    enemyTextures[3] = loadSprite("resources/enemies/boss/atlas.png");
    hostileShotTexture = loadSprite("resources/projectiles/hostile/atlas.png");
    npcTextures[0] = loadSprite("resources/npcs/watchman/atlas.png");
    npcTextures[1] = loadSprite("resources/npcs/healer/atlas.png");
    Scenery::load();
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
    input.attack = IsMouseButtonDown(MOUSE_BUTTON_LEFT);
    input.switchWeapon = IsKeyPressed(KEY_Q);
    input.dash = IsKeyPressed(KEY_SPACE);
    input.reload = IsKeyPressed(KEY_R);
    input.interact = IsKeyPressed(KEY_E);
    input.attributes = IsKeyPressed(KEY_TAB);
    input.mouse = GetMousePosition();
    input.movement = {
        static_cast<float>(IsKeyDown(KEY_D) - IsKeyDown(KEY_A)),
        static_cast<float>(IsKeyDown(KEY_S) - IsKeyDown(KEY_W))
    };
}

void Game::changeState(GameState nextState)
{
    state = nextState;
    selectedOption = 0;
    showingInstructions = false;
}

void Game::startNewGame()
{
    elapsedTime = 0.0f;
    dialogueLine = 0;
    dungeon = Dungeon();
    player = Player(dungeon.currentRoom().getSpawn());
    combat = Combat();
    camera = dungeon.currentRoom().getCamera(GetScreenWidth(), GetScreenHeight(), player.getPosition());
    changeState(GameState::Playing);
}

Rectangle Game::optionBounds(int index) const
{
    if (state == GameState::Menu && !showingInstructions)
        return {76,350.0f+index*59,340,48};
    return {Config::windowWidth / 2.0f - 170, 290.0f + index * 64, 340, 50};
}

int Game::readMenuChoice(int optionCount)
{
    if (input.up) selectedOption = (selectedOption + optionCount - 1) % optionCount;
    if (input.down) selectedOption = (selectedOption + 1) % optionCount;

    for (int i = 0; i < optionCount; ++i)
    {
        if (input.click && CheckCollisionPointRec(GetScreenToWorld2D(input.mouse,getUiCamera()), optionBounds(i)))
        {
            selectedOption = i;
            return i;
        }
    }
    return input.confirm ? selectedOption : -1;
}

void Game::update(float deltaTime)
{
    if (state != GameState::Menu)
        camera = dungeon.currentRoom().getCamera(GetScreenWidth(),GetScreenHeight(),player.getPosition());
    switch (state)
    {
        case GameState::Menu:
        {
            if (showingInstructions)
            {
                if (input.escape || (input.click && CheckCollisionPointRec(GetScreenToWorld2D(input.mouse,getUiCamera()), optionBounds(4))))
                    showingInstructions = false;
                else if (input.confirm || input.down || input.click) helpPage = (helpPage+1)%3;
                else if (input.up) helpPage = (helpPage+2)%3;
                break;
            }
            const int choice = readMenuChoice(3);
            if (choice == 0) startNewGame();
            else if (choice == 1) { showingInstructions = true; helpPage = 0; }
            else if (choice == 2) exitRequested = true;
            break;
        }
        case GameState::Playing:
            if (input.escape)
            {
                changeState(GameState::Paused);
                break;
            }
            if (input.attributes)
            {
                changeState(GameState::Attributes);
                break;
            }
            if (input.interact)
            {
                const Npc& npc = dungeon.currentRoom().getNpc();
                if (npc.type != NpcType::None && Vector2Distance(player.getPosition(), npc.position) <= Config::interactionRadius)
                {
                    dialogueLine = 0;
                    changeState(GameState::Dialogue);
                    break;
                }
                if (dungeon.currentRoom().interactLever(player.getPosition())) input.interact = false;
            }
            if (input.switchWeapon) combat.switchWeapon();
            if (input.reload) combat.reload();
            // Passos curtos preservam duracoes e colisoes em quadros lentos.
            for (float remaining = deltaTime; remaining > 0;)
            {
                const float step = std::min(remaining, 1.0f / 240);
                elapsedTime += step;
                dungeon.currentRoom().getNpc().update(step);
                player.aimAt(GetScreenToWorld2D(input.mouse, camera));
                player.update(input.movement, step, dungeon.currentRoom(), input.dash);
                input.dash = false;
                camera = dungeon.currentRoom().getCamera(GetScreenWidth(), GetScreenHeight(), player.getPosition());
                player.aimAt(GetScreenToWorld2D(input.mouse, camera));
                collectItems(dungeon.currentRoom(), player, combat, input.interact);
                input.interact = false;
                const int previousLevel = player.getLevel();
                dungeon.currentRoom().updateEnemyDeaths(step);
                combat.update(step, input.attack, player, dungeon.currentRoom());
                for (Enemy& enemy : dungeon.currentRoom().getEnemies()) enemy.update(step, dungeon.currentRoom(), player);
                dungeon.currentRoom().updateHazards(step, player);
                dungeon.currentRoom().updateEnemyShots(step, player);
                dungeon.currentRoom().updateWaves(step, player);
                if (player.getHealth() <= 0)
                {
                    changeState(GameState::Defeat);
                    break;
                }
                if (player.hasWon())
                {
                    changeState(GameState::Victory);
                    break;
                }
                if (dungeon.update(step, player))
                {
                    combat.clearTransient();
                    camera = dungeon.currentRoom().getCamera(GetScreenWidth(), GetScreenHeight(), player.getPosition());
                }
                if (player.getLevel() > previousLevel)
                {
                    changeState(GameState::Attributes);
                    break;
                }
                remaining -= step;
            }
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
        {
            if (input.escape || input.attributes)
            {
                changeState(GameState::Playing);
                break;
            }
            const int choice = readMenuChoice(4);
            if (choice >= 0 && choice < 3) player.spendPoint(static_cast<Attribute>(choice));
            else if (choice == 3) changeState(GameState::Playing);
            break;
        }
        case GameState::Dialogue:
            dungeon.currentRoom().getNpc().update(deltaTime,true,dialogueLine);
            if (input.interact || input.confirm || input.escape)
            {
                ++dialogueLine;
                if (dialogueLine >= 3)
                {
                    dungeon.finishDialogue(player);
                    changeState(GameState::Playing);
                }
            }
            break;
        case GameState::Victory:
        case GameState::Defeat:
        {
            if (state == GameState::Defeat) player.updateDeathAnimation(deltaTime);
            else dungeon.currentRoom().updateEnemyDeaths(deltaTime);
            const int choice = readMenuChoice(2);
            if (choice == 0) startNewGame();
            else if (choice == 1 || input.escape) changeState(GameState::Menu);
            break;
        }
    }
}

Camera2D Game::getUiCamera()
{
    const int width = GetScreenWidth() > 0 ? GetScreenWidth() : Config::windowWidth;
    const int height = GetScreenHeight() > 0 ? GetScreenHeight() : Config::windowHeight;
    const float scale = Config::windowScale(width,height);
    return {{(width-Config::windowWidth*scale)/2,(height-Config::windowHeight*scale)/2},{},0,scale};
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
        const bool resume = (state == GameState::Paused && i == 0) ||
                            (state == GameState::Attributes && i == 3);
        const bool selected = i == selectedOption;
        const Color highlight{222,192,122,255};
        DrawRectangleRec(bounds, selected ? highlight : resume ? Color{35,62,55,255} : Config::panel);
        if (selected)
        {
            DrawRectangleLinesEx(bounds,3,Config::text);
            DrawText(">",static_cast<int>(bounds.x)-30,static_cast<int>(bounds.y)+10,28,highlight);
        }
        else if (resume) DrawRectangleLinesEx(bounds,1,Config::accent);
        drawCentered(options[i],static_cast<int>(bounds.y)+14,22,
            selected ? Config::background : resume ? Config::accent : Config::text);
    }
}

void Game::drawWorld() const
{
    const float scale = Config::windowScale(GetScreenWidth(),GetScreenHeight());
    BeginScissorMode(0, static_cast<int>(Config::hudHeight*scale), GetScreenWidth(),
                     static_cast<int>(GetScreenHeight() - (Config::hudHeight + Config::footerHeight)*scale));
    BeginMode2D(camera);
    dungeon.currentRoom().draw();
    dungeon.drawDetails();
    const NpcType npcType = dungeon.currentRoom().getNpc().type;
    dungeon.currentRoom().drawInteractions(player.getPosition(),
        npcType == NpcType::None ? Texture2D{} : npcTextures[npcType == NpcType::Healer ? 1 : 0]);
    drawItems(dungeon.currentRoom(), player, combat);
    dungeon.currentRoom().drawWaveWarning();
    combat.draw(player, dungeon.currentRoom(),player.getHealth() > 0 &&
        !(IsTextureValid(knightTexture) && player.usesSwordAttackSprite()),enemyTextures,hostileShotTexture);
    player.draw(knightTexture);
    dungeon.currentRoom().drawExploration();
    if (dungeon.currentRoom().getId() == 12)
        for (const Enemy& enemy : dungeon.currentRoom().getEnemies())
            if (enemy.getType() == EnemyType::Boss && dungeon.currentRoom().isVisible(enemy.getPosition()))
                DrawText("Guardiao corrompido", enemy.getPosition().x-115, enemy.getPosition().y-94, 18, GOLD);
    EndMode2D();
    EndScissorMode();
    Camera2D hudCamera = getUiCamera();
    hudCamera.offset.y = 0;
    BeginMode2D(hudCamera);
    DrawText("FORTALEZA CORROMPIDA",20,8,20,Config::accent);
    DrawText(TextFormat("Escudo: %i",player.getShield()),730,8,18,Config::text);
    DrawText(TextFormat("Nivel %i | XP %i/%i | Pontos %i (Tab) | Pontuacao %i", player.getLevel(),
             player.getExperience(), player.getNextLevelCost(), player.getAttributePoints(),
             player.getScore(dungeon.getDiscoveredCount())), 20, 35, 18, Config::text);
    DrawText(TextFormat("Sala %i | Chave: %s | Bonus %.1fs | Tempo %.1fs",
             dungeon.currentRoom().getId(), dungeon.hasKey() ? "sim" : "nao",
             player.getBonusRemaining(), elapsedTime), 20, 62, 16, Config::muted);
    if (dungeon.currentRoom().getWaveCount() > 0)
        DrawText(TextFormat("Onda %i/%i", dungeon.currentRoom().getWaveNumber(), dungeon.currentRoom().getWaveCount()),
                 575,62,16,ORANGE);
    if (dungeon.currentRoom().isWaveWaiting())
        drawCentered(TextFormat("Reforcos em %.1f s",dungeon.currentRoom().getWaveRemaining()),110,22,ORANGE);
    if (dungeon.currentRoom().hasLivingEnemies())
        DrawText("Portas: elimine os inimigos", 690, 62, 16, RED);
    else if (dungeon.nearLockedDoor(player.getPosition()))
        DrawText("Trancada: chave na sala 9", 690, 62, 16, GOLD);
    DrawText("WASD: mover | Clique: atacar | Q: arma | R: recarregar | Espaco: dash | E: interagir | Tab | Esc", 20,
             static_cast<int>(GetScreenHeight()/scale) - 34, 16, Config::muted);
    EndMode2D();
    drawStatusPanels();
}

void Game::drawStatusPanels() const
{
    const float scale = Config::windowScale(GetScreenWidth(),GetScreenHeight());
    Camera2D view{};
    view.zoom = scale;
    BeginMode2D(view);
    const float y = GetScreenHeight()/scale - Config::footerHeight - 112;
    const Color border{92,94,83,255}, track{12,17,20,255};
    DrawRectangleRec({20,y,254,100},Fade(Config::background,0.95f));
    DrawRectangleLinesEx({20,y,254,100},1,border);
    DrawText("VIDA",34,y+12,16,Config::text);
    const std::string health = std::to_string(player.getHealth())+" / "+std::to_string(player.getMaxHealth());
    DrawText(health.c_str(),260-MeasureText(health.c_str(),16),y+12,16,Config::text);
    DrawRectangleRec({34,y+34,226,12},track);
    DrawRectangleRec({34,y+34,226*static_cast<float>(player.getHealth())/player.getMaxHealth(),12},{182,75,69,255});
    DrawText("DASH",34,y+57,14,Config::muted);
    const bool ready = player.getDashCooldown() <= 0;
    const char* dash = ready ? "PRONTO" : TextFormat("%.1f s",player.getDashCooldown());
    DrawText(dash,260-MeasureText(dash,14),y+57,14,ready ? Config::accent : Config::muted);
    DrawRectangleRec({34,y+79,226,7},track);
    const float dashFill = std::clamp(1-player.getDashCooldown()/Config::dashCooldown,0.0f,1.0f);
    DrawRectangleRec({34,y+79,226*dashFill,7},Config::accent);
    const float x = GetScreenWidth()/scale-274;
    DrawRectangleRec({x,y,254,100},Fade(Config::background,0.95f));
    DrawRectangleLinesEx({x,y,254,100},1,border);
    DrawText(weaponName(combat.getWeapon().type),x+16,y+12,20,Config::text);
    drawWeapon(combat.getWeapon().type,{x+36,y+55},{1,0});
    if (combat.getWeapon().category == WeaponCategory::Ranged)
    {
        DrawText(TextFormat("%02i / %i",combat.getAmmo(),magazineCapacity(combat.getWeapon().type)),
            x+105,y+40,24,combat.getAmmo() == 0 ? ORANGE : Config::text);
        if (combat.getReloadRemaining() > 0)
        {
            DrawText("RECARREGANDO",x+105,y+68,12,ORANGE);
            DrawRectangleRec({x+16,y+89,222,3},track);
            DrawRectangleRec({x+16,y+89,222*(1-combat.getReloadRemaining()/reloadDuration(combat.getWeapon().type)),3},ORANGE);
        }
        else DrawText("R: recarregar | reserva infinita",x+16,y+82,12,Config::muted);
    }
    else DrawText("CORPO A CORPO",x+105,y+48,14,Config::muted);
    if (combat.getWeapon().category == WeaponCategory::Melee)
        DrawText("Q: trocar arma",x+105,y+74,12,Config::muted);
    EndMode2D();
}

void Game::drawInstructions() const
{
    const char* titles[] = {"Como jogar", "Itens e atributos", "Pontuacao"};
    drawCentered(titles[helpPage], 65, 36, Config::accent);
    if (helpPage == 0)
    {
        drawCentered("Liberte o guardiao corrompido na sala 12.", 145, 22, Config::text);
        drawCentered("Explore as salas. A chave da sala 9 abre o portao da sala 7.", 195, 20, Config::text);
        drawCentered("WASD: mover | Mouse: mirar | Clique: atacar", 255, 20, Config::text);
        drawCentered("Q: armas | R: recarregar | Espaco: dash | E: interagir", 300, 20, Config::text);
        drawCentered("Tab: distribuir atributos | Esc: pausar", 345, 20, Config::text);
        drawCentered("Caminhe sobre portas numeradas. Ao morrer, a partida termina.", 405, 18, Config::muted);
        drawCentered("Fale com o vigia e com o curandeiro no refugio da sala 8.", 445, 18, Config::muted);
    }
    else if (helpPage == 1)
    {
        drawCentered("Kit: cura ate 30 | Escudo: absorve 30 de dano", 145, 20, Config::text);
        drawCentered("Bonus roxo: +25% de dano por 10 segundos", 190, 20, Config::text);
        drawCentered("Itens comuns: toque | Armas: E (deixa a anterior no chao)", 235, 18, Config::text);
        drawCentered("Espada / lanca: corpo a corpo | Pistola / escopeta: tiros", 280, 18, Config::text);
        drawCentered("Nivel: custo de 50 x nivel atual. Cada subida rende 1 ponto.", 325, 18, Config::text);
        drawCentered("Forca: +10% dano | Vitalidade: +20 vida e cura", 370, 20, Config::text);
        drawCentered("Agilidade: +5% velocidade. Tab abre a distribuicao.", 415, 20, Config::text);
        drawCentered("Espinhos: cinza descansa, amarelo avisa, vermelho fere.", 460, 18, Config::muted);
    }
    else
    {
        drawCentered("+100 por inimigo | +50 por coleta efetiva", 155, 22, Config::text);
        drawCentered("+200 por sala descoberta | +1000 pela vitoria", 205, 22, Config::text);
        drawCentered("-1 a cada 100 unidades percorridas (inclui dash)", 265, 20, Config::text);
        drawCentered("-2 por ponto de vida realmente recuperado", 315, 20, Config::text);
        drawCentered("Curas de kits, curandeiro e Vitalidade entram na conta.", 365, 18, Config::muted);
        drawCentered("Pontuacao minima: zero. Retornar a uma sala nao soma descoberta.", 415, 18, Config::muted);
        drawCentered("XP: 20 por inimigo comum e 100 pelo guardiao.", 460, 20, Config::text);
    }
    drawCentered(TextFormat("Pagina %i/3 | Enter / setas: mudar | Esc: voltar", helpPage+1), 510, 18, GOLD);
    const Rectangle back = optionBounds(4);
    DrawRectangleRec(back, Config::panel);
    DrawRectangleLinesEx(back, 2, Config::accent);
    drawCentered("Voltar", static_cast<int>(back.y)+14, 22, Config::accent);
}

void Game::draw() const
{
    BeginDrawing();
    ClearBackground(Config::background);
    if (state == GameState::Menu)
    {
        BeginMode2D(getUiCamera());
        if (showingInstructions) drawInstructions();
        else drawMainMenu();
        EndMode2D();
    }
    else
    {
        drawWorld();
        if (state != GameState::Playing)
        {
            DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Config::overlay);
            BeginMode2D(getUiCamera());
            if (state == GameState::Paused)
            {
                drawCentered("Pausa", 180, 40, Config::accent);
                const char* options[] = {"Retomar (Esc)", "Reiniciar partida", "Voltar ao menu"};
                drawOptions(options, 3);
            }
            else if (state == GameState::Dialogue)
            {
                drawCentered(dungeon.currentRoom().getNpc().type == NpcType::Healer ? "Curandeiro" : "Vigia", 210, 36, Config::accent);
                drawCentered(dialogueText(), 310, 20, Config::text);
                drawCentered("E / Enter / Esc: continuar", 400, 20, GOLD);
            }
            else if (state == GameState::Attributes)
            {
                drawCentered("Distribuicao de atributos", 120, 36, Config::accent);
                const Attributes& attributes = player.getAttributes();
                drawCentered(TextFormat("Nivel %i | XP %i/%i | Pontos disponiveis: %i", player.getLevel(),
                    player.getExperience(), player.getNextLevelCost(), player.getAttributePoints()), 180, 22, Config::text);
                drawCentered(TextFormat("Forca %i | Vitalidade %i | Agilidade %i", attributes.strength,
                    attributes.vitality, attributes.agility), 220, 20, Config::muted);
                const char* options[] = {"Forca: +10% dano base", "Vitalidade: +20 vida e cura",
                    "Agilidade: +5% velocidade", "Retomar (Tab / Esc)"};
                drawOptions(options, 4);
                drawCentered("W/S ou setas: selecionar | Enter: aplicar | Tab/Esc: retomar",565,16,Config::muted);
            }
            else
            {
                drawCentered(state == GameState::Victory ? "Vitoria" : "Derrota",
                             180, 40, Config::accent);
                drawCentered(TextFormat("Pontuacao final: %i", player.getScore(dungeon.getDiscoveredCount())),
                    235, 24, Config::text);
                const char* options[] = {"Reiniciar partida", "Voltar ao menu"};
                drawOptions(options, 2);
            }
            EndMode2D();
        }
    }
    EndDrawing();
}

void Game::shutdown()
{
    Scenery::unload();
    if (IsTextureValid(knightTexture)) UnloadTexture(knightTexture);
    knightTexture = {};
    if (IsTextureValid(hostileShotTexture)) UnloadTexture(hostileShotTexture);
    hostileShotTexture = {};
    for (Texture2D& texture : npcTextures)
    {
        if (IsTextureValid(texture)) UnloadTexture(texture);
        texture = {};
    }
    for (Texture2D& texture : enemyTextures)
    {
        if (IsTextureValid(texture)) UnloadTexture(texture);
        texture = {};
    }
    CloseWindow();
}

const char* Game::dialogueText() const
{
    const Npc& npc = dungeon.currentRoom().getNpc();
    if (npc.type == NpcType::Watchman)
    {
        const char* lines[] = {"A corrupcao tomou o guardiao desta fortaleza.",
            "Encontre a chave na sala 9 para abrir o portao da sala 7.",
            "Derrote o guardiao na sala 12. O curandeiro espera na sala 8."};
        return lines[dialogueLine];
    }
    const char* lines[] = {"O guardiao tentou conter a corrupcao e acabou dominado.",
        npc.healingUsed ? "Minha cura ja foi usada. Proteja sua vida." : "Posso recuperar toda a sua vida, uma unica vez.",
        "Abri um atalho entre este refugio e a sala 4. Prepare-se."};
    return lines[dialogueLine];
}

void Game::drawMainMenu() const
{
    const Color ink{13,17,20,255}, stone{49,53,52,255}, stoneLight{70,73,66,255};
    const Color paper{222,212,184,255}, ochre{187,143,76,255}, dust{132,132,116,255};
    ClearBackground(ink);
    // Gravura geometrica da entrada, desenhada junto com a interface.
    DrawRectangleGradientH(460,0,500,640,ink,{29,32,30,255});
    const Vector2 vanishing{726,400};
    for (int x=460; x<1100; x+=80) DrawLineEx(vanishing,{static_cast<float>(x),640},1,{39,42,39,255});
    for (int y : {450,485,530,590}) DrawLine(475,y,960,y,{39,42,39,255});
    DrawRectangleRec({553,156,350,320},{22,26,25,255});
    for (int row=0; row<9; ++row)
        for (int column=0; column<6; ++column)
        {
            const float x=550+column*61-(row%2)*30, y=146+row*37;
            if (x<548 || x>880) continue;
            DrawRectangleRec({x,y,58,34},(row+column)%3 == 0 ? stone : Color{39,44,42,255});
            DrawLine(x,y,x+58,y,Fade(stoneLight,0.5f));
        }
    DrawRectangleRec({536,133,50,347},stone);
    DrawRectangleRec({863,133,50,347},stone);
    DrawRectangleRec({529,127,64,17},stoneLight);
    DrawRectangleRec({856,127,64,17},stoneLight);
    for (int x=551; x<904; x+=44) DrawRectangleRec({static_cast<float>(x),109,29,46},stone);
    DrawCircle(725,274,104,stoneLight);
    DrawCircle(725,274,84,{8,11,12,255});
    DrawRectangleRec({621,274,208,206},stoneLight);
    DrawRectangleRec({641,274,168,206},{8,11,12,255});
    DrawRectangleGradientV(649,283,152,190,{12,16,17,255},{53,41,28,255});
    DrawTriangle({650,472},{800,472},{805,600},Fade(ochre,0.08f));
    for (int x=655; x<805; x+=19) DrawRectangle(x,257,5,213,{24,28,27,255});
    DrawRectangle(645,314,160,8,{30,33,30,255});
    DrawRectangle(645,411,160,8,{30,33,30,255});
    for (int step=0; step<4; ++step)
    {
        DrawRectangleRec({600.0f-step*16,478.0f+step*16,250.0f+step*32,15},step%2 ? stone : stoneLight);
        DrawLine(600-step*16,478+step*16,850+step*16,478+step*16,{86,85,72,255});
    }
    for (int x : {588,851})
    {
        DrawCircleGradient({static_cast<float>(x),335},63,Fade(ochre,0.14f),Fade(ochre,0));
        DrawLineEx({static_cast<float>(x),340},{static_cast<float>(x),365},5,{97,68,43,255});
        DrawPoly({static_cast<float>(x),334},3,11,180,ochre);
        DrawPoly({static_cast<float>(x),334},3,5,180,paper);
    }
    DrawRectangle(72,70,32,3,ochre);
    DrawText("ROGUE LIKE",116,65,16,dust);
    DrawText("FORTALEZA",73,147,48,paper);
    DrawText("DO GUARDIAO",76,201,32,paper);
    DrawText("Encontre a chave.",78,267,18,dust);
    DrawText("Enfrente o guardiao.",78,291,18,dust);
    const char* labels[] = {"Entrar na fortaleza","Como jogar","Sair"};
    for (int i=0; i<3; ++i)
    {
        const Rectangle area=optionBounds(i);
        const bool selected=i==selectedOption;
        if (selected)
        {
            DrawRectangleRec(area,{39,40,34,255});
            DrawRectangleRec({area.x,area.y,3,area.height},ochre);
            DrawText(">",static_cast<int>(area.x+17),static_cast<int>(area.y+14),20,ochre);
        }
        DrawText(labels[i],static_cast<int>(area.x+42),static_cast<int>(area.y+14),20,selected ? paper : dust);
    }
    DrawLine(76,555,416,555,{53,56,49,255});
    DrawText("W/S ou setas para escolher",78,574,14,dust);
    DrawText("Enter para confirmar  /  Mouse",78,596,14,dust);
}
