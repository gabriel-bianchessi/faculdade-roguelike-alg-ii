# Cenario

Piso, alvenaria, pilar, caixa e quatro portais recortados de
`Câmara de Pedra com Quatro Portais.png`, na raiz. Os pixels da arte original
sao preservados. `src/Scenery.h` compartilha as oito texturas entre as salas;
o jogo carrega e libera os recursos junto com a janela e usa filtro de ponto.

Para refazer os recortes, execute na raiz:

```sh
g++ -std=c++20 -Ibuild/external/raylib-master/src scripts/extract-scenery.cpp bin/Debug/libraylib.a -lX11 -lpthread -lm -ldl -lrt -o /tmp/rogue-like-extract-scenery
/tmp/rogue-like-extract-scenery
```

O piso repete o recorte com espelhamento alternado nas bordas. Paredes e
obstaculos seguem seus retangulos de colisao; as caixas destruidas deixam
de ser desenhadas. Os portais ocupam exatamente as areas de transicao,
mantendo numero da sala e indicador verde/vermelho de bloqueio. A sala do
curandeiro usa uma tonalidade esverdeada no piso. Sem os PNGs, o desenho
geometrico continua disponivel.
