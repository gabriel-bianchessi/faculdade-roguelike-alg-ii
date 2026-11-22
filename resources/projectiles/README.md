# Projéteis hostis

Fonte: `Spritesheet de Projéteis Mágicos Roxos.png`, fornecida na raiz. `hostile/atlas.png` contém quatro colunas e oito linhas de células transparentes de 256 × 256 pixels. Os 32 PNGs também ficam em `hostile/frames/`.

As linhas seguem norte, nordeste, leste, sudeste, sul, sudoeste, oeste e noroeste. Cada direção tem quatro quadros, trocados a cada 0,08 s de simulação. A direção é escolhida a partir da velocidade real do disparo; o núcleo claro da imagem fica alinhado à posição usada pela colisão. O jogo desenha cada célula numa área de 40 × 40 pixels, com fundo transparente.

A textura é usada nos disparos do chefe e do besteiro. Dano, velocidade, alcance e colisão permanecem iguais. O relógio da animação congela nas pausas. As imagens são desenhadas antes da sombra do cenário. Os projéteis do jogador mantêm o visual dourado.

O extrator é `scripts/extract-boss.cpp`, documentado em `resources/enemies/boss/README.md`. Os atlas e quadros prontos acompanham a entrega; não é necessário executar o extrator para jogar.
