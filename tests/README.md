# Validação

Na raiz do projeto:

```sh
bash tests/run.sh
```

O script compila o jogo, compila os testes com C++20 e `-Wall -Wextra -Wshadow` e executa todos sem janela. Objetos e executáveis temporários são removidos ao terminar. A flag `-fno-access-control` permite aos testes de fluxo inspecionar `Game` sem ampliar a interface pública do jogo.

| Arquivo | Cobertura |
|---|---|
| movement.cpp | Movimento cardinal/diagonal em 15, 30, 60, 144 e 240 FPS; cantos, colisão varrida, deslizamento, mira e câmera em quatro resoluções |
| combat.cpp | Dano único por golpe, área, cadência, tiros, paredes, invulnerabilidade, dash e perseguidor |
| combat-flow.cpp | Seleção por teclado preservada ao passar o mouse, seleção/confirmação por clique em todos os menus, pausa, temporizadores congelados, morte, reinício, tiros ao trocar de sala, movimento contínuo após o atalho 8–4 e interação única por quadro |
| exploration.cpp | Todas as conexões, chegadas livres, acessibilidade, fog, persistência, chave, porta trancada e bloqueio por inimigos vivos (retorno, ramificação, atalho e chefe) |
| items.cpp | Cura efetiva, vida cheia, escudo, renovação/expiração, contagem, troca e dano fracionário das armas |
| enemy-animation.cpp | Três atlas e 90 PNGs, caminhada direcional, ataque móvel/cadência, dano, morte sem XP duplicado, portas, pausa, sala inativa e reinício |
| player-animation.cpp | Atlas e 30 PNGs, caminhada nas quatro direções, repouso, espada, dano, pausa, morte e reinício |
| progression.cpp | XP excedente/múltiplos níveis, pontos, atributos derivados, cura, distância real, fórmula, pausa e reset |
| interactions.cpp | Caixas e grade, ataques de ambos os lados, ciclos de espinhos, alavancas, diálogos, cura única e atalho |
| navigation.cpp | BFS contornando parede, caminho sem saída/reaberto, atirador, patrulheiro, vaguear e 40 entidades |
| dungeon-content.cpp | 100 sementes, posições válidas, ausência de sobreposição, salas seguras, conexões das 13 salas incluindo tesouro e reprodutibilidade |
| boss-visuals.cpp | Atlas, 48 quadros do chefe, 32 dos projéteis, oito direções, relógio de animação, pausa e reinício |
| boss.cpp | Aviso, investida, leque, fase acelerada, colisão, 100 XP, vitória única e reinício |
| vision-waves.cpp | Visão direcional, oclusão por paredes/caixas, memória, zoom, ondas finitas, surgimento seguro, pausa e persistência |
| full-run.cpp | Partida real simulada: movimento até portas, combates, 13 descobertas, chave, armas, companheiro, tesouro, NPC, atributos, chefe, vitória, derrota e reinícios |

`full-run.cpp` usa uma semente fixa para ser reproduzível. O controlador escolhe entradas de movimento, mira, ataque e menus; não teleporta, altera vida, concede dano nem adiciona recursos na partida vitoriosa. A derrota posterior usa um perseguidor de teste junto ao jogador parado para verificar o fluxo de morte.

Para verificar acessos inválidos e comportamento indefinido:

```sh
bash tests/run.sh --sanitize
```

Neste ambiente, LeakSanitizer é incompatível com o ptrace usado pelo executor. A verificação com AddressSanitizer e UndefinedBehaviorSanitizer passou com:

```sh
ASAN_OPTIONS=detect_leaks=0 bash tests/run.sh --sanitize
```

Isso não verifica vazamentos. A inspeção de propriedade confirma que as entidades usam valores/`std::vector`, sem `new`/`delete` manuais. Testes lógicos não substituem inspeção visual: menu, três páginas da ajuda, HUD, atributos, diálogo, armadilhas, aviso do chefe e resultado também foram abertos e conferidos em janela de 960 × 640.

As regressões de `movement.cpp` mantêm pressão por oito segundos contra faces
fracionárias e verificam saída imediata no sentido oposto em quatro taxas de quadros.
`ammunition.cpp` cobre pentes 15/7, um cartucho por leque, recarga manual e automática,
reserva infinita, troca de armas/salas, pausa, diálogo e reinício.
`npc-visuals.cpp` valida os 32 recortes, transparência e margens, gestos de diálogo,
efeito de cura única, congelamento em pausa e reinício.
O navegador da partida completa aproxima alvos em células bloqueadas da grade
pelo centro livre mais próximo com visada, simulando contorno manual do obstáculo.


Testes da parte 2:

| Arquivo | Cobertura |
|---|---|
| difficulty.cpp | 300 partidas semeadas: progressão preservada, probabilidades de recursos/tipos/quantidades e seleção de dificuldade |
| ranking.cpp | Tempo na fórmula, persistência/ordenação, arquivo ausente/corrompido, falha de gravação e registro único por partida |
| part2-items.cpp | Cristal de XP, histórico de armas/itens, inventário congelando o mundo e reset |
| enemy-items.cpp | Destruição de consumíveis, efeito transitório, preservação de armas/chave e bloqueio por paredes |
| companion.cpp | Recrutamento, BFS em volta de obstáculo, destino em célula bloqueada, colisão, mortes/chefe, XP único, pausa, transição e reset |
| bonus-room.cpp | Segredo por proximidade, ligação única, bloqueio em combate, recompensa/XP, persistência e identificação de fases |

A partida completa também recruta o vigia e visita o tesouro. Esse controlador existe apenas nos testes e não implementa o modo Computador/NPC solicitado separadamente pelo enunciado.
