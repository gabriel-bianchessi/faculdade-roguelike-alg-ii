Verificacao do movimento e das colisoes (apos `make`), a partir da raiz:

```sh
g++ -std=c++20 -Wall -Wextra -Wshadow -Isrc -isystem build/external/raylib-master/src tests/movement.cpp src/Player.cpp src/Room.cpp bin/Debug/libraylib.a -lX11 -lpthread -lm -ldl -lrt -o /tmp/rogue-like-movement-test
/tmp/rogue-like-movement-test
```

Os testes nao abrem janela. Comparam movimento a 15, 30, 60, 144 e 240 FPS,
colisoes com paredes e obstaculos, deslizamento, cantos, mira e ajuste da camera.
