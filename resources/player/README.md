# Cavaleiro

Fonte: `sprites-knight.png`, imagem fornecida para este projeto.

`knight.png` é o atlas usado pelo jogo: quatro colunas, oito linhas e células transparentes de 192 × 128 pixels. A âncora do corpo fica em (80, 116), junto aos pés. O jogo desenha a 40% do tamanho, com filtro de vizinho mais próximo.

| Linha | Animação | Quadros |
|---|---|---|
| 0 | Parado | 4 |
| 1 | Andar para baixo | 4 |
| 2 | Andar para a esquerda | 4 |
| 3 | Andar para a direita | 4 |
| 4 | Andar para cima | 4 |
| 5 | Golpe de espada | 4 |
| 6 | Receber dano | 2 |
| 7 | Morrer | 4 |

Os 30 quadros também estão separados em `frames/`. As legendas e linhas da tabela foram excluídas; resíduos com alfa menor que 96 foram removidos. O golpe da imagem é usado com a espada, espelhado quando o personagem mira para a esquerda. Lança, pistola e escopeta mantêm seus desenhos e animações por código. A prioridade visual é morte, dano, golpe e movimento; as animações congelam em pausa. A morte termina no último quadro durante a tela de derrota.

Para reproduzir a separação na raiz do projeto, após compilar o jogo:

```sh
g++ -std=c++20 -Ibuild/external/raylib-master/src scripts/extract-knight.cpp bin/Debug/libraylib.a -lX11 -lpthread -lm -ldl -lrt -o /tmp/rogue-extract-knight
/tmp/rogue-extract-knight
```

Os PNGs prontos acompanham o projeto e o pacote de entrega; não é necessário executar o extrator para jogar. A textura é carregada uma única vez após abrir a janela e liberada antes de fechá-la. Reiniciar a partida reinicia apenas o estado da animação.
