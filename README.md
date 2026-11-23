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

O Premake baixa a Raylib quando `build/external/raylib-master` não existe, exigindo internet nessa primeira configuração. O pacote de entrega inclui os fontes dessa dependência para compilação sem esse download. O projeto usa a versão de Raylib fornecida pelo template; os sprites acompanham `resources/`, e o jogo não depende de downloads de imagens ou áudio durante a execução. Ao adicionar arquivos C++, gere novamente os arquivos com Premake.

## Controles

| Entrada | Ação |
|---|---|
| WASD | Mover, com diagonais normalizadas |
| Mouse / botão esquerdo | Mirar / atacar continuamente |
| R | Recarregar o pente da arma de tiros |
| Q | Alternar entre arma corpo a corpo e arma de tiros |
| Espaço | Dash, invulnerável na primeira metade |
| E | Conversar, acionar alavanca ou recolher a arma diferente mais próxima |
| Tab | Abrir ou fechar a distribuição de atributos |
| I | Abrir ou fechar equipamento e histórico de coletas |
| Esc | Pausar ou retomar; avançar diálogo; voltar na ajuda |
| Setas ou W/S e Enter | Selecionar opções dos menus |

A vida e a recuperação do dash aparecem em barras no canto inferior esquerdo. O painel inferior direito mostra a arma equipada e a munição: pistola com 15 tiros e escopeta com 7 cartuchos. A reserva é infinita; `R` recarrega (1,2 s na pistola e 1,8 s na escopeta), e um pente vazio inicia a recarga automaticamente. Cada disparo da escopeta consome um cartucho para seus cinco projéteis. Trocar de arma ou sala preserva os pentes; as recargas avançam durante a partida, inclusive com a arma guardada, e congelam em pausas e diálogos.

O vigia e o curandeiro usam sprites animados recortados das folhas originais; veja [os detalhes dos NPCs](resources/npcs/README.md).

No menu de instruções, Enter e setas alternam as três páginas: controles, itens/atributos e pontuação. Também é possível selecionar as opções com o mouse.

## Exploração e combate

O caminho principal é `1 → 2 → 3 → 4 → 5 → 6 → 7 → 12`. As ramificações são `2 → 8`, `3 → 9`, `5 → 10` e `6 → 11`. Caminhe sobre uma porta numerada para atravessar. Todas as passagens da sala ficam bloqueadas enquanto houver inimigos vivos; ficam vermelhas e reabrem ao eliminar o grupo atual. A chave da sala 9 abre o portão entre 7 e 12, nos dois sentidos e sem ser consumida.

O vigia está na entrada segura da sala 1 e acompanha o jogador após concluir o diálogo, contornando obstáculos e ajudando no combate. O curandeiro da sala 8 recupera toda a vida uma vez e abre um atalho persistente até a sala 4. Caixas podem ser destruídas por ataques de jogador e inimigos; alavancas desligam os espinhos associados. Espinhos cinza descansam, amarelos avisam e vermelhos causam dano. Inimigos derrotados, itens, exploração e mecanismos permanecem alterados ao retornar.

O jogo abre em janela maximizada e redimensionável. Menus, HUD, cliques e câmera acompanham o tamanho da janela. A câmera usa zoom base de 1,0× em 960 × 640, escalado proporcionalmente em janelas maiores. A visão acompanha a mira em um campo de 220° com alcance de 460 unidades; paredes e caixas projetam sombras. As áreas fora da iluminação ficam mais claras, preservando a memória da exploração. Inimigos continuam visíveis fora do campo de visão, escurecidos pela mesma sombra do cenário.

Há perseguidores, atiradores e patrulheiros rápidos. Atiradores continuam se aproximando enquanto disparam, respeitando as colisões. Eles vagueiam fora do alcance de percepção e usam busca em largura na grade para contornar obstáculos. Posições de inimigos e recursos são sorteadas entre candidatos válidos no início de cada partida. As salas 2 e 3 têm duas ondas; as demais salas de combate comuns têm três. Os reforços chegam após eliminar o grupo atual, com aviso de 3 segundos e surgimento distante do jogador. A onda e seu temporizador persistem nas revisitas e param durante pausas. As salas 1 e 8 são seguras. O chefe da sala 12 alterna perseguição, investida anunciada e tiros em leque; abaixo da metade da vida seus intervalos diminuem.

| Item/arma | Efeito |
|---|---|
| Kit verde | Recupera até 30; permanece se a vida estiver cheia |
| Escudo azul | Absorve 30; recoleta renova, sem somar |
| Bônus roxo | +25% de dano por 10 segundos; recoleta renova |
| Cristal azul de XP | Concede 30 XP; o cristal do tesouro concede 100 |
| Espada inicial | 20 de dano, intervalo de 0,4 s |
| Pistola inicial | 8 de dano, intervalo de 0,3 s |
| Lança da sala 10 | 28 de dano, golpe longo/estreito, intervalo de 0,65 s |
| Escopeta da sala 11 | 5 projéteis de 6 de dano, intervalo de 0,9 s |

Itens comuns são coletados por contato. Armas exigem E: uma troca por acionamento, deixando a anterior no chão. Há um espaço para cada categoria; a reserva de munição é infinita, com pentes e recarga. Inimigos comuns destroem consumíveis ao encostar; armas e chave são preservadas. Pausa, diálogos e distribuição de atributos suspendem os temporizadores e o combate.

## Experiência e pontuação

Começa-se no nível 1, com 60 de vida e velocidade de 240 unidades/s. Cada inimigo comum concede 20 XP e o chefe, 100. Subir exige `50 × nível atual`, preservando excedente, e concede um ponto:

- Força: +10% do dano base por ponto, antes do bônus temporário.
- Vitalidade: +20 de vida máxima e cura de 20.
- Agilidade: +5% da velocidade base por ponto.

A distribuição abre ao subir e pode ser acessada por Tab. Pontos podem ser guardados.

```text
máximo(0,
  100 × inimigos derrotados + 50 × coletas efetivas + 200 × salas descobertas
  + 1000 × vitória − piso(distância real / 100) − 2 × vida recuperada − piso(tempo ativo / 10))
```

Distância inclui dash e exclui teleporte/pressão contra paredes. Cura considera apenas vida realmente recuperada por kits, curandeiro e Vitalidade. Descoberta e vitória contam uma vez. O HUD mostra os valores da partida, e vitória/derrota exibem o resultado final.

## Parte 2

Antes de começar, escolha Fácil, Normal ou Difícil. A seleção muda a quantidade sorteada de inimigos, a proporção entre tipos e a chance de recursos. A dificuldade aparece no HUD e acompanha o resultado no ranking.

O menu **Ranking** mostra todas as partidas concluídas em páginas de oito, ordenadas pela pontuação decrescente (empate: menor duração). Vitória e derrota são registradas uma vez. O arquivo fica em `$XDG_DATA_HOME/rogue-like/ranking.txt` ou, normalmente, `~/.local/share/rogue-like/ranking.txt`. Ele não salva o andamento da partida. Pausas, diálogos, inventário e atributos não contam no tempo ativo.

**I** mostra as duas armas, efeitos atuais e histórico dos nove tipos obtidos (oito itens/armas mais a chave). O bônus de dano deixa o cavaleiro roxo enquanto durar. O vigia é um companheiro invulnerável e não bloqueia o jogador: segue a 220 unidades/s, tenta manter 72 unidades de distância e ajuda com 8 de dano a cada 0,8 s em um inimigo visível por linha reta a até 180 unidades. As mortes continuam concedendo XP e pontuação ao jogador uma única vez.

As três etapas são a entrada (salas 1–3), a fortaleza interna e suas ramificações (4–11) e o santuário do chefe (12). No depósito, sala 10, examine com **E** a rachadura próxima ao canto inferior direito para revelar a sala bônus 13. Ela contém XP, escudo e cura; os recursos não reaparecem nas revisitas.

Áudio permanece adiado. O modo em que o computador joga sozinho será especificado separadamente.

## Verificação e organização

```sh
bash tests/run.sh
# Em ambientes que permitem LeakSanitizer:
bash tests/run.sh --sanitize
# Sob depurador/ptrace, sem verificação de vazamentos:
ASAN_OPTIONS=detect_leaks=0 bash tests/run.sh --sanitize
```

Os testes verificam regras isoladas, persistência, 100 sementes de surgimentos e uma partida completa por entradas simuladas, seguida de derrota e reinícios. Detalhes em [tests/README.md](tests/README.md). Critérios do enunciado e evidências estão em [docs/VALIDACAO.md](docs/VALIDACAO.md); roteiro e exercícios para a defesa em [docs/DEFESA.md](docs/DEFESA.md).

`Game` coordena os estados; `Player` concentra atributos e estatísticas; `Room` mantém cenário, grade e entidades; `Dungeon` conecta as salas; `Combat` resolve golpes e tiros; `Enemy` diferencia comportamentos por enum. Configurações ficam em `src/Config.h` e `src/Weapon.h`. O plano e o andamento estão em [anotacoes.md](anotacoes.md) e [TODO.md](TODO.md). A nova etapa tem checklist própria em [TODO-parte-2.md](TODO-parte-2.md) e regras em [docs/PARTE-2.md](docs/PARTE-2.md).

Para preparar um arquivo com fontes, testes, documentos e a dependência Raylib:

```sh
bash scripts/package.sh
```

O arquivo gerado fica em `entrega/rogue-like.tar.gz`. A publicação e a apresentação são feitas pelos integrantes; confira [docs/ENTREGA.md](docs/ENTREGA.md).

O menu traz uma ilustração geométrica da entrada da fortaleza. As quatro armas têm desenhos próprios no chão e nas mãos do personagem, acompanhando a mira, com animação de golpe ou recuo/clarão ao disparar. As armas são desenhadas pela Raylib. O jogador usa o cavaleiro da imagem `sprites-knight.png`, com 30 quadros separados em `resources/player/frames/` e um atlas em `resources/player/knight.png`: parado, caminhada nas quatro direções, golpe de espada, dano e morte. A textura é carregada automaticamente ao iniciar o jogo, inclusive ao executar pelo caminho do binário. Detalhes e comando para reproduzir a separação estão em [resources/player/README.md](resources/player/README.md).

Os três inimigos comuns também usam sprites animadas: cavaleiro corrompido (perseguidor), besteiro (atirador) e patrulheiro sombrio. São 90 quadros separados em `resources/enemies/`, com caminhada direcional, ataques, dano e morte. As animações de morte não bloqueiam portas nem duplicam XP. Consulte [resources/enemies/README.md](resources/enemies/README.md) para a organização e o extrator. O chefe usa 48 quadros próprios, incluindo avisos, investida e magia. Os projéteis roxos dos inimigos usam 32 quadros em oito direções; detalhes em [resources/enemies/boss/README.md](resources/enemies/boss/README.md) e [resources/projectiles/README.md](resources/projectiles/README.md).

O template de compilação é [raylib-quickstart](https://github.com/raylib-extras/raylib-quickstart), de Jeffery Myers, disponibilizado sob CC0. A licença da Raylib é incluída na dependência. Esta versão salva o ranking, mas não o progresso de partidas. Não inclui multiplayer, geração procedural de salas nem áudio; áudio está adiado na checklist da parte 2.
