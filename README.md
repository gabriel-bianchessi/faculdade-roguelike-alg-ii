# Rogue Like — A fortaleza do guardião

Jogo de exploração e combate em tempo real, feito em C++20 com Raylib para o trabalho de Algoritmos e Programação II. O explorador precisa encontrar a chave e libertar o guardião corrompido da fortaleza. A morte encerra a partida; reiniciar restaura todo o mundo.

## Compilar e executar

Alvo validado: Linux x86_64, teclado e mouse, ambiente gráfico com OpenGL 3.3. São necessários GCC com C++20, Make e as bibliotecas de desenvolvimento de X11/OpenGL. Em Ubuntu/Debian, os pacotes são `build-essential`, `libgl1-mesa-dev`, `libx11-dev`, `libxcursor-dev`, `libxrandr-dev`, `libxinerama-dev` e `libxi-dev`.

Na primeira compilação, gere os arquivos do Make:

```sh
cd build
./premake5 gmake
cd ..
make -j2
./bin/Debug/rogue-like
```

Se os arquivos já foram gerados:

```sh
make && ./bin/Debug/rogue-like
```

O Premake baixa a Raylib quando `build/external/raylib-master` não existe, exigindo internet nessa primeira configuração. O pacote de entrega inclui os fontes dessa dependência para compilação sem esse download. O projeto usa a versão de Raylib fornecida pelo template; o jogo não depende de imagens, fontes ou áudio externos. Ao adicionar arquivos C++, gere novamente os arquivos com Premake.

## Controles

| Entrada | Ação |
|---|---|
| WASD | Mover, com diagonais normalizadas |
| Mouse / botão esquerdo | Mirar / atacar continuamente |
| Q | Alternar entre arma corpo a corpo e arma de tiros |
| Espaço | Dash, invulnerável na primeira metade |
| E | Conversar, acionar alavanca ou recolher a arma diferente mais próxima |
| Tab | Abrir ou fechar a distribuição de atributos |
| Esc | Pausar ou retomar; avançar diálogo; voltar na ajuda |
| Setas ou W/S e Enter | Selecionar opções dos menus |

No menu de instruções, Enter e setas alternam as três páginas: controles, itens/atributos e pontuação. Também é possível selecionar as opções com o mouse.

## Exploração e combate

O caminho principal é `1 → 2 → 3 → 4 → 5 → 6 → 7 → 12`. As ramificações são `2 → 8`, `3 → 9`, `5 → 10` e `6 → 11`. Caminhe sobre uma porta numerada para atravessar. Todas as passagens da sala ficam bloqueadas enquanto houver inimigos vivos; ficam vermelhas e reabrem ao eliminar o grupo atual. A chave da sala 9 abre o portão entre 7 e 12, nos dois sentidos e sem ser consumida.

O vigia está na entrada segura da sala 1. O curandeiro da sala 8 recupera toda a vida uma vez e abre um atalho persistente até a sala 4. Caixas podem ser destruídas por ataques de jogador e inimigos; alavancas desligam os espinhos associados. Espinhos cinza descansam, amarelos avisam e vermelhos causam dano. Inimigos derrotados, itens, exploração e mecanismos permanecem alterados ao retornar.

A câmera usa zoom de 1,0×. A visão acompanha a mira em um campo de 220° com alcance de 460 unidades; paredes e caixas projetam sombras. As áreas fora da iluminação ficam mais claras, preservando a memória da exploração. Inimigos continuam visíveis fora do campo de visão, escurecidos pela mesma sombra do cenário.

Há perseguidores, atiradores e patrulheiros rápidos. Atiradores continuam se aproximando enquanto disparam, respeitando as colisões. Eles vagueiam fora do alcance de percepção e usam busca em largura na grade para contornar obstáculos. Posições de inimigos e recursos são sorteadas entre candidatos válidos no início de cada partida. As salas 2 e 3 têm duas ondas; as demais salas de combate comuns têm três. Os reforços chegam após eliminar o grupo atual, com aviso de 3 segundos e surgimento distante do jogador. A onda e seu temporizador persistem nas revisitas e param durante pausas. As salas 1 e 8 são seguras. O chefe da sala 12 alterna perseguição, investida anunciada e tiros em leque; abaixo da metade da vida seus intervalos diminuem.

| Item/arma | Efeito |
|---|---|
| Kit verde | Recupera até 30; permanece se a vida estiver cheia |
| Escudo azul | Absorve 30; recoleta renova, sem somar |
| Bônus roxo | +25% de dano por 10 segundos; recoleta renova |
| Espada inicial | 20 de dano, intervalo de 0,4 s |
| Pistola inicial | 8 de dano, intervalo de 0,3 s |
| Lança da sala 10 | 28 de dano, golpe longo/estreito, intervalo de 0,65 s |
| Escopeta da sala 11 | 5 projéteis de 6 de dano, intervalo de 0,9 s |

Itens comuns são coletados por contato. Armas exigem E: uma troca por acionamento, deixando a anterior no chão. Há um espaço para cada categoria; munição é ilimitada. Pausa, diálogos e distribuição de atributos suspendem os temporizadores e o combate.

## Experiência e pontuação

Começa-se no nível 1, com 60 de vida e velocidade de 240 unidades/s. Cada inimigo comum concede 20 XP e o chefe, 100. Subir exige `50 × nível atual`, preservando excedente, e concede um ponto:

- Força: +10% do dano base por ponto, antes do bônus temporário.
- Vitalidade: +20 de vida máxima e cura de 20.
- Agilidade: +5% da velocidade base por ponto.

A distribuição abre ao subir e pode ser acessada por Tab. Pontos podem ser guardados.

```text
máximo(0,
  100 × inimigos derrotados + 50 × coletas efetivas + 200 × salas descobertas
  + 1000 × vitória − piso(distância real / 100) − 2 × vida recuperada)
```

Distância inclui dash e exclui teleporte/pressão contra paredes. Cura considera apenas vida realmente recuperada por kits, curandeiro e Vitalidade. Descoberta e vitória contam uma vez. O HUD mostra os valores da partida, e vitória/derrota exibem o resultado final.

## Verificação e organização

```sh
bash tests/run.sh
# Em ambientes que permitem LeakSanitizer:
bash tests/run.sh --sanitize
# Sob depurador/ptrace, sem verificação de vazamentos:
ASAN_OPTIONS=detect_leaks=0 bash tests/run.sh --sanitize
```

Os testes verificam regras isoladas, persistência, 100 sementes de surgimentos e uma partida completa por entradas simuladas, seguida de derrota e reinícios. Detalhes em [tests/README.md](tests/README.md). Critérios do enunciado e evidências estão em [docs/VALIDACAO.md](docs/VALIDACAO.md); roteiro e exercícios para a defesa em [docs/DEFESA.md](docs/DEFESA.md).

`Game` coordena os estados; `Player` concentra atributos e estatísticas; `Room` mantém cenário, grade e entidades; `Dungeon` conecta as salas; `Combat` resolve golpes e tiros; `Enemy` diferencia comportamentos por enum. Configurações ficam em `src/Config.h` e `src/Weapon.h`. O plano e o andamento estão em [anotacoes.md](anotacoes.md) e [TODO.md](TODO.md).

Para preparar um arquivo com fontes, testes, documentos e a dependência Raylib:

```sh
bash scripts/package.sh
```

O arquivo gerado fica em `entrega/rogue-like.tar.gz`. A publicação e a apresentação são feitas pelos integrantes; confira [docs/ENTREGA.md](docs/ENTREGA.md).

O menu traz uma ilustração geométrica da entrada da fortaleza. As quatro armas têm desenhos próprios no chão e nas mãos do personagem, acompanhando a mira, com animação de golpe ou recuo/clarão ao disparar. As armas são desenhadas pela Raylib. O jogador usa o cavaleiro da imagem `sprites-knight.png`, com 30 quadros separados em `resources/player/frames/` e um atlas em `resources/player/knight.png`: parado, caminhada nas quatro direções, golpe de espada, dano e morte. A textura é carregada automaticamente ao iniciar o jogo, inclusive ao executar pelo caminho do binário. Detalhes e comando para reproduzir a separação estão em [resources/player/README.md](resources/player/README.md).

Os três inimigos comuns também usam sprites animadas: cavaleiro corrompido (perseguidor), besteiro (atirador) e patrulheiro sombrio. São 90 quadros separados em `resources/enemies/`, com caminhada direcional, ataques, dano e morte. As animações de morte não bloqueiam portas nem duplicam XP. Consulte [resources/enemies/README.md](resources/enemies/README.md) para a organização e o extrator.

O template de compilação é [raylib-quickstart](https://github.com/raylib-extras/raylib-quickstart), de Jeffery Myers, disponibilizado sob CC0. A licença da Raylib é incluída na dependência. Esta versão não inclui salvamento, multiplayer, geração procedural de salas nem áudio.
