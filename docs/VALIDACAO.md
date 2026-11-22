# Conferência do enunciado e do plano

O PDF de referência é `T3 - Roguelike - 2026.pdf`. Esta matriz relaciona os critérios com implementação e verificações concretas. Execução local: `bash tests/run.sh`; testes com AddressSanitizer/UndefinedBehaviorSanitizer também passaram com `ASAN_OPTIONS=detect_leaks=0`. LeakSanitizer não foi validado porque o executor usa ptrace.

## Funcionalidades do PDF

| Critério | Implementação | Evidência |
|---|---|---|
| Otimização e movimento sem engasgos excessivos | Passos de até 1/240 s, só sala atual ativa, caminhos por intervalo | `movement`, `navigation` (40 entidades), `full-run`; janela aberta |
| Sem piscadas ou deslocamentos incoerentes | Desenho único por quadro; câmera limitada; colisões varridas | Testes de câmera/colisão e inspeção de telas em 960×640 |
| Design consistente | Paleta em Config, silhuetas por tipo, grade, fog, avisos e efeitos | Menu, HUD, atributos, NPCs, espinhos e chefe inspecionados em janela |
| Menu funcional com instruções, itens e pontuação | Menu e três páginas de ajuda em Game | Todas as páginas renderizadas e conferidas; testes de fluxo |
| Movimento em todas as direções | WASD normalizado pelo tempo | `movement` em cinco taxas de quadros |
| Bloqueio por paredes | Room::move e sólidos persistentes | `movement`, `combat`, `navigation` |
| Tipos de inimigos, vaguear/perseguir e colisões | Chaser, Shooter e Patrol; BFS | `navigation`, `dungeon-content`, `full-run` |
| Vitória no último nível ao derrotar chefe | Chefe exclusivo da sala 12; marcação única | `boss` e vitória em `full-run` |
| Itens, chaves e portas | Kit, escudo, bônus, quatro armas e chave não consumida | `items`, `exploration`, `full-run` |
| Derrota por vida zero | GameState::Defeat | `combat-flow`, `full-run` |
| Mapa escuro, revelado por exploração | Células visíveis/exploradas persistentes | `exploration` e janela |
| Sistema de batalha | Golpes orientados, projéteis, dano, cadência, dash e invulnerabilidade | `combat`, `items`, `boss` |
| Pontuação e vida na tela | HUD e resultado final | `progression`, `boss`, inspeção visual |
| Armadilhas | Espinhos cíclicos em 4–7 e alavancas | `interactions`, janela |
| Experiência e níveis | 20/100 XP; custo 50×nível; excedente | `progression`, `boss`, `full-run` |
| Atributos distribuídos por nível | Tela pausada, pontos guardáveis, Tab | `progression` e janela |
| Atributos alteram comportamento | Força/dano, Vitalidade/vida, Agilidade/velocidade | `progression` e dano de arma após investir |
| História e NPCs | Vigia, curandeiro, corrupção do guardião | `interactions`, diálogos e ajuda |
| Cenário muda com NPC, inimigo e jogador | Atalho 8–4, caixas destruídas por ambos, alavancas | `interactions`, ida/volta e reinício |
| Boss final | Perseguição, avisos, investida, leque e fase acelerada | `boss`, `full-run` e janela |

## Técnicas de programação do PDF

| Critério | Evidência no código |
|---|---|
| Structs | Attributes, WeaponConfig, Projectile, Item, Connection, Cell, Box, Spikes, Lever, Npc |
| Sub-rotinas e parâmetros/referências | Entrada/update/draw separados; Player::heal, Room::findPath, Combat::resolveSwing, Dungeon::update |
| Ponteiros e referências | Dungeon::findRoom e nearbyWeapon retornam ponteiros opcionais sem propriedade; Room/Player passados por referência; leitura por const referência |
| Segmentação e emprego das técnicas | Game coordena; Player, Enemy, Room, Dungeon e Combat separam responsabilidades; Geometry compartilha colisão de segmento |

A revisão de invalidação conferiu: dados da arma são copiados antes de apagar/inserir no vetor; ponteiro do alvo do projétil não sobrevive à remoção de inimigos; salas são montadas antes de criar conexões por ID; atualização de caixas altera sólidos/grade, sem realocar inimigos. As entidades não usam `new`/`delete` manuais.

## Requisitos adicionais do plano

- As 12 salas têm dimensões diferentes, caminho principal, quatro ramificações e atalho de NPC. As entradas são livres e os destinos acessíveis; 100 sementes foram verificadas.
- Grupos e posições das ondas são sorteados na construção da Dungeon. Reforços chegam após limpar o grupo anterior, com aviso de 3 segundos, distância segura e validação dos sólidos atuais; `vision-waves` cobre pausa e persistência e `dungeon-content` verifica as ondas em 100 sementes. Salas seguras não recebem inimigos. Retornos preservam mortes, itens, fog, caixas, alavancas, NPC e chave.
- Espada, pistola, lança e escopeta respeitam alcance, cadência, dano fracionário e dois espaços de inventário. Uma interação não recolhe a arma descartada.
- Escudos/bônus renovam; kits com vida cheia permanecem; efeitos congelam na pausa.
- XP conta uma vez ao remover a morte. Cura efetiva, distância incluindo dash, coletas, descobertas e vitória entram na fórmula com mínimo zero.
- A partida automatizada percorreu todas as salas, coletou a chave e as armas opcionais, conversou com o curandeiro, distribuiu pontos e venceu sem teleporte/dano/vida artificiais. Depois verificou derrota e reinício. A semente fixa torna o caso reproduzível; isso não representa todas as estratégias humanas ou todos os sorteios.
- O balanceamento conserva as bases do plano. A entrada e o refúgio são seguros, kits e Vitalidade dão recuperação limitada, e o chefe avisa seus ataques. O teste vitorioso confirma que a progressão é possível; novas partidas humanas podem motivar ajustes posteriores.

## Menu, vida e desenhos das armas

A vida inicial e os reinícios agora usam 60. Testes de cura, Vitalidade, dano e reinício foram ajustados para verificar os valores concretos dessa base. A partida completa também venceu com essa vida inicial.

O menu foi redesenhado com uma entrada de fortaleza, cores de pedra/latão e navegação alinhada à esquerda; clique em ajuda, retorno e início foram conferidos em janela. Espada, pistola, lança e escopeta usam o mesmo desenho no chão e equipadas, rotacionado pela mira. Os quatro modelos, golpe da espada, estocada, recuo e clarão de disparo foram renderizados e inspecionados. As mudanças nas armas são visuais; os valores de dano e cadência continuam os do plano.

## Ajustes de visão e ondas

A visão passou de raio circular de 240 para campo de 220° guiado pela mira, com alcance de 460, percepção próxima de 110 e oclusão por paredes e caixas. O zoom passou de 1,2× para 1,0×. Sombras de áreas exploradas usam alfa 125 (antes 180); áreas desconhecidas usam 205 (antes 255). Testes verificam giro da mira, memória, bloqueio e liberação da visão após destruir caixa.

A partida automatizada passou também com ondas: 65 inimigos derrotados, vitória, derrota e reinícios. O controlador aguarda os reforços antes de prosseguir. As ondas são finitas e não são reiniciadas por revisitas.

## Conferência do pacote

A versão inicial do pacote de entrega foi extraída numa pasta temporária limpa. `./premake5 gmake` gerou os arquivos sem baixar dependências, `make -j2` compilou do zero e `bash tests/run.sh` passou também nessa cópia. A janela do jogo abriu e fechou normalmente; uma amostra de 120 quadros do cenário do chefe levou 1,98 s com limite de 60 FPS neste computador. Esse valor é uma amostra local, não uma garantia de desempenho em qualquer máquina.

## Limites da evidência e tarefas humanas

Testes e inspeção visual dão evidência dos casos descritos, não uma prova de ausência de todo bug possível. Não há avaliação humana registrada da dificuldade ou da fluidez em outros computadores.

`DEFESA.md` contém o roteiro e os exercícios, mas cada integrante ainda precisa ensaiar a explicação e realizar as alterações individualmente. `ENTREGA.md` orienta o envio. O pacote local é preparado e pode ser compilado após extração; autoria, postagem e apresentação ao professor dependem da equipe. Essas atividades não estão marcadas como executadas na TODO.

As passagens ficam bloqueadas enquanto houver inimigos vivos na sala, com indicação vermelha nas portas e no HUD. `exploration` verifica retorno, ramificação, atalho e portão do chefe, incluindo reabertura após a morte do grupo mesmo com inimigos mortos ainda na coleção. Inimigos fora do campo de visão são desenhados antes da camada de sombra, permanecendo visíveis com a iluminação do cenário.

O cavaleiro fornecido em `sprites-knight.png` foi separado em 30 PNGs transparentes e um atlas 4 × 8 em `resources/player/`. `player-animation` valida os arquivos, quatro direções, repouso, golpe de espada, dano, pausa, morte e reinício. A textura, o encaixe da pistola para a esquerda, o golpe espelhado e a tela de derrota foram conferidos em janela. A textura pertence a `Game`, permanece carregada nos reinícios e é liberada no encerramento; os testes lógicos não exigem contexto gráfico. Os recursos e a folha original acompanham o pacote.

As folhas de cavaleiro corrompido, besteiro e patrulheiro foram integradas aos três inimigos comuns, com 90 quadros separados e três atlas em `resources/enemies/`. `enemy-animation` valida arquivos, direções, ataque móvel do atirador sem alterar a cadência, dano, morte visual, XP único, portas liberadas, pausa, sala inativa e reinício. A partida completa simulada continua passando. Em janela foram conferidos os três modelos na escala do cavaleiro, caminhada, disparo para a esquerda, dano, morte, sombra fora da visão e liberação das quatro texturas no encerramento. O chefe recebeu posteriormente uma folha própria, descrita abaixo.

A janela agora abre maximizada e redimensionável. Menus, HUD, câmera e áreas de clique usam escala proporcional; o escurecimento da pausa cobre a janela inteira. Foram conferidos abertura maximizada em 1920×992, restauração em 1280×720, cliques de ajuda/início/retorno e mira. `movement` verifica a câmera e a conversão da mira em quatro resoluções.

A folha do chefe foi separada em 48 quadros, ligados aos estados de perseguição, aviso de investida, investida, aviso de magia, disparo, recuperação, dano e morte. A morte visual continua durante a vitória sem alterar XP ou pontuação. A folha de projéteis foi separada em 32 quadros para oito direções, com núcleo alinhado à colisão. `boss-visuals` verifica arquivos, direções e pausa; `boss` verifica a correspondência dos estados e a morte na vitória.

## Correções de colisão, HUD, pentes e NPCs

- Reproduzida a penetração por arredondamento em faces fracionárias; teste de contato prolongado e saída imediata passa após comparar centro e faces expandidas com a mesma expressão.
- Pentes de 15 tiros na pistola e 7 cartuchos na escopeta; reserva infinita, recarga manual em R e automática ao esvaziar, temporizadores preservados na troca e congelados em pausa/diálogo. Um leque consome um cartucho.
- Vigia e curandeiro com 16 quadros cada, incluindo conversa, gestos e efeito da cura única. Recortes, transparência, congelamento e reinício cobertos em teste.
- Interface conferida em janela maximizada (1920 × 992) e mínima (960 × 640): barras inferiores à esquerda e painel da arma com munição e progresso de recarga à direita, sem sobreposição do rodapé. Texturas carregam e são descarregadas no encerramento.
- Partida completa automatizada conclui as 12 salas com o novo sistema de munição. O navegador de teste agora contorna obstáculos quando a célula da grade do alvo está bloqueada.

Validação desta rodada: `bash tests/run.sh`, `ASAN_OPTIONS=detect_leaks=0 bash tests/run.sh --sanitize` e `git diff --check` passaram.
