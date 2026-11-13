Verificacao do movimento e das colisoes (apos `make`), a partir da raiz:

```sh
g++ -std=c++20 -Wall -Wextra -Wshadow -Isrc -isystem build/external/raylib-master/src tests/movement.cpp src/Player.cpp src/Room.cpp bin/Debug/libraylib.a -lX11 -lpthread -lm -ldl -lrt -o /tmp/rogue-like-movement-test
/tmp/rogue-like-movement-test
```

Os testes nao abrem janela. Comparam movimento a 15, 30, 60, 144 e 240 FPS,
colisoes com paredes e obstaculos, deslizamento, cantos, mira e ajuste da camera.


Verificacao do combate:

```sh
g++ -std=c++20 -Wall -Wextra -Wshadow -Isrc -isystem build/external/raylib-master/src tests/combat.cpp src/Combat.cpp src/Enemy.cpp src/Player.cpp src/Room.cpp bin/Debug/libraylib.a -lX11 -lpthread -lm -ldl -lrt -o /tmp/rogue-like-combat-test
/tmp/rogue-like-combat-test
```

Verifica area e dano unico da espada, cadencia, troca de armas, tiros bloqueados,
remocao de mortos, dash em diferentes FPS, invulnerabilidade e ataque do perseguidor.

Fluxo de pausa, derrota e reinicio (sem janela):

```sh
g++ -std=c++20 -Wall -Wextra -Wshadow -fno-access-control -Isrc -isystem build/external/raylib-master/src tests/combat-flow.cpp src/Game.cpp src/Combat.cpp src/Enemy.cpp src/Player.cpp src/Room.cpp bin/Debug/libraylib.a -lX11 -lpthread -lm -ldl -lrt -o /tmp/rogue-like-combat-flow-test
/tmp/rogue-like-combat-flow-test
```

A flag `-fno-access-control` permite ao teste inspecionar os estados privados de
`Game`, sem expor esses detalhes na interface usada pelo jogo.
