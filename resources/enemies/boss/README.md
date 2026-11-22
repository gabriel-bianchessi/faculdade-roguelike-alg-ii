# Guardião corrompido

Fonte: `Spritesheet do Cavaleiro Sombrio Corrompido.png`, fornecida na raiz. O atlas `atlas.png` contém quatro colunas e 12 linhas de células transparentes de 256 × 256 pixels. Os 48 quadros também estão separados em `frames/`. A âncora fica em (128, 236), junto aos pés, com escala de 65% e filtro de vizinho mais próximo.

| Linha | Animação |
|---|---|
| 0 | Parado |
| 1–4 | Caminhar para baixo, esquerda, direita e cima |
| 5 | Preparar investida |
| 6 | Investir |
| 7 | Preparar magia |
| 8 | Disparar magia |
| 9 | Recuperar o equilíbrio |
| 10 | Receber dano |
| 11 | Morrer |

As animações acompanham os estados existentes do chefe. A linha vermelha da investida e o círculo dourado da magia continuam indicando os ataques. O aviso percorre os quadros dentro da duração normal ou enfurecida. Vida, dano, colisões, velocidades e cadências permanecem iguais. A morte continua durante a tela de vitória e concede XP apenas uma vez.

Para reproduzir os recortes do chefe e dos projéteis, após compilar o jogo, execute na raiz:

```sh
g++ -std=c++20 -Ibuild/external/raylib-master/src scripts/extract-boss.cpp bin/Debug/libraylib.a -lX11 -lpthread -lm -ldl -lrt -o /tmp/rogue-extract-boss
/tmp/rogue-extract-boss
```
