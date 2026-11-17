# Inimigos

As três folhas fornecidas na raiz foram separadas preservando o desenho original:

| Pasta | Inimigo no jogo | Fonte |
|---|---|---|
| `chaser` | Perseguidor | `Spritesheet de Cavaleiro Corrompido.png` |
| `shooter` | Atirador | `Spritesheet do Besteiro Corrompido.png` |
| `patrol` | Patrulheiro rápido | `Spritesheet do Patrulheiro Sombrio.png` |

Cada pasta contém `atlas.png` e 30 PNGs separados em `frames/`. O atlas tem quatro colunas e oito linhas, com células transparentes de 256 × 256 pixels e âncora do corpo em (128, 236), junto aos pés. As linhas são: parado, andar para baixo, esquerda, direita e cima, atacar, receber dano e morrer. Dano tem dois quadros; as demais linhas têm quatro. A área não usada fica transparente.

O jogo usa filtro de vizinho mais próximo e escala de 24% para perseguidor/atirador e 26% para patrulheiro. Ataques são espelhados para a esquerda. Os quadros de movimento seguem o deslocamento real, incluindo os desvios dos obstáculos. Receber dano tem prioridade sobre atacar e caminhar.

Morte é apenas uma animação de 0,64 s: o inimigo deixa de causar dano e bloquear portas imediatamente, concede XP uma única vez e permanece numa coleção visual separada até terminar a animação. Essa animação congela ao pausar, abrir atributos ou sair da sala. Todos os inimigos continuam desenhados sob a sombra fora do campo de visão.

As texturas pertencem a `Game`, são carregadas uma única vez e liberadas antes do fechamento da janela. Os comportamentos, vida, velocidades e cadências dos inimigos permanecem os mesmos. O chefe continua usando seu desenho geométrico, pois não foi fornecida uma folha para ele.

Os arquivos prontos acompanham a entrega. Para reproduzir a separação a partir das folhas originais, após compilar o projeto, execute na raiz:

```sh
g++ -std=c++20 -Ibuild/external/raylib-master/src scripts/extract-enemies.cpp bin/Debug/libraylib.a -lX11 -lpthread -lm -ldl -lrt -o /tmp/rogue-extract-enemies
/tmp/rogue-extract-enemies
```

O extrator remove pixels com alfa menor que 96 e um fragmento do efeito de disparo que invade a célula seguinte na folha do besteiro. As folhas originais são mantidas na raiz e incluídas no pacote.
