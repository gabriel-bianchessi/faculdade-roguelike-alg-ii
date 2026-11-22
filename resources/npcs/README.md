# Vigia e curandeiro

Recortes das folhas originais `Guarda medieval em animação pixel art.png` e
`Spritesheet do Curandeiro Medieval.png`, preservadas na raiz.
Cada personagem possui 16 quadros e um atlas de 1536 × 1536 pixels:
quatro colunas, quatro linhas, células de 384 × 384, âncora dos pés (192, 360).
O jogo usa escala 0,16 e filtro pontual; transparências residuais abaixo de 96
são removidas pelo recortador, sem redesenhar os personagens.

| Linha | Vigia | Curandeiro |
|---|---|---|
| 0 | Repouso | Repouso |
| 1 | Conversa | Conversa |
| 2 | Aponta o caminho | Cura |
| 3 | Despedida | Retorno ao repouso |

O vigia gesticula conforme a fala. O curandeiro executa cura e retorno ao repouso
após concluir seu primeiro diálogo; a cura e o atalho mantêm as regras existentes.
As animações de conversa avançam durante o diálogo sem atualizar o combate.
Pausa e atributos congelam os quadros e os efeitos; reiniciar limpa o estado.

Para reproduzir os recortes, na raiz:

```bash
g++ -std=c++20 -isystem build/external/raylib-master/src scripts/extract-npcs.cpp bin/Debug/libraylib.a -lX11 -lpthread -lm -ldl -lrt -o /tmp/rogue-extract-npcs
/tmp/rogue-extract-npcs
```
