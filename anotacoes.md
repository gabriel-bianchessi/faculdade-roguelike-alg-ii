# Rogue Like

Rogue like simples seguindo as diretrizes do PDF. 

## Mapa

O mapa será feito de maneira manual. Ele possuirá 12 diferentes salas, que serão as fases, ao final da linha principal 
de salas estará a sala do chefe. Dentro das salas existirão armas e kits de saúde para coletar, inimigos e obstáculos 
naturais do cenário. Essas salas terão tamanhos diferenciados entre si.

## Gráficos

Os gráficos serão bem simples, basicamente polígonos desenhados na tela, o personagem principal terá uma espada e uma 
pistola que dá pouco dano, eles serão representados por formas simples.

## Armas e combate

O menu usa uma entrada de fortaleza desenhada em geometria, tons de pedra e latão e opções alinhadas à esquerda. As quatro armas têm modelos próprios desenhados em Raylib, usados no chão e equipados, orientados pela mira. Espada e lança têm animações de golpe; pistola e escopeta exibem recuo e clarão de disparo.

O jogo possuirá diversas armas, sendo elas de curto e longo alcance. Existirão armas de combate corpo a corpo e à 
distância. O personagem terá um dash que na primeira metade do dash ele ficará invulnerável.

---

# Plano ampliado

## Objetivo

Criar um jogo em C++ com Raylib, visão superior, combate em tempo real e gráficos geométricos, cobrindo todos os critérios do PDF. Usar classes para comportamentos e structs para dados simples, mantendo o código acessível para a defesa.

A partida começa com espada e pistola, atravessa 12 salas manuais e termina ao derrotar o chefe. A morte reinicia toda a partida.

## Mapa e exploração

- Caminho principal: `1 → 2 → 3 → 4 → 5 → 6 → 7 → 12`.
- Ramificações: `2 → 8`, `3 → 9`, `5 → 10` e `6 → 11`.
- Sala 1: início e NPC vigia; sala 8: área segura com curandeiro; sala 9: chave da passagem entre 7 e 12; salas 10 e 11: armas e recursos opcionais; sala 12: chefe.
- Permitir retorno, preservando inimigos derrotados, itens coletados e alterações no cenário.
- Criar salas de dimensões diferentes com paredes sólidas, caixas destrutíveis, portas, alavancas e armadilhas. Bloquear as passagens da sala enquanto houver inimigos vivos, indicando o bloqueio em vermelho.
- Câmera acompanha o jogador com zoom fixo de 1,0×, limitada às bordas da sala; não é necessário mostrar a sala inteira.
- Revelar células de 32 unidades num campo de visão de 220° orientado pela mira, com alcance de 460 unidades e percepção próxima de 110 unidades ao redor do jogador. Paredes e caixas bloqueiam a visão. Regiões exploradas ficam sob sombra de alfa 125; regiões desconhecidas sob alfa 205, permitindo perceber fracamente o cenário. Inimigos fora da visão continuam desenhados sob a sombra do cenário.
- Caminhar sobre uma porta numerada troca de sala. As chegadas ficam fora do gatilho da porta, com intervalo mínimo de 0,3 segundo entre transições; o dash é interrompido ao entrar. A chave não é consumida ao atravessar a porta 7–12.
- Sortear inimigos e itens entre posições válidas predefinidas no início da partida, sem bloquear entradas. Salas 2 e 3 recebem duas ondas no total; salas 4–7 e 9–11 recebem três, com grupos crescentes. Após eliminar o grupo atual, há aviso de 3 segundos para os reforços. O surgimento respeita distância mínima de 220 unidades do jogador e sólidos atuais. Pausas e salas inativas congelam a contagem; retornar não reinicia ondas já vencidas. Salas 1, 8 e a arena do chefe não recebem reforços.

## Controles e combate

- `WASD`: movimentar; mouse: mirar; botão esquerdo: atacar.
- `Q`: alternar entre as duas armas; `Espaço`: dash; `E`: interagir ou coletar armas; `Esc`: pausar; `Tab`: abrir a distribuição de atributos.
- Normalizar movimento diagonal e controlar velocidades e temporizadores pelo tempo entre quadros.
- Dash de 0,24 segundo, invulnerável na primeira metade, com recarga de 1 segundo e colisão com paredes.
- Após sofrer dano, conceder 0,5 segundo de invulnerabilidade. Não somar esse tempo ao do dash.
- Espada: alcance de 76 unidades, largura de 64 e golpe ativo por 0,12 segundo. Pistola: projéteis a 650 unidades por segundo, raio de 4 e duração máxima de 2 segundos.
- Lança: alcance de 130 unidades e largura de 28. Escopeta: cinco projéteis distribuídos num leque de 0,5 radiano, com a mesma velocidade e duração dos tiros da pistola. O bônus de dano é aplicado ao disparar ou iniciar o golpe; danos fracionários são preservados.
- Perseguidor inicial: 40 de vida, velocidade de 120 unidades por segundo e 10 de dano a cada 0,8 segundo. Dash: velocidade de 800 unidades por segundo.
- Golpes atingem uma área à frente do personagem, uma vez por alvo em cada ataque. Projéteis desaparecem ao atingir parede ou alvo.
- Carregar uma arma corpo a corpo e uma à distância. Substituir uma arma deixa a anterior no chão. Munição ilimitada, limitada pela cadência.

| Arma | Dano base | Intervalo | Comportamento |
|---|---:|---:|---|
| Espada | 20 | 0,4 s | Golpe curto e largo |
| Lança | 28 | 0,65 s | Golpe longo e estreito |
| Pistola | 8 | 0,3 s | Um projétil |
| Escopeta | 6 por projétil | 0,9 s | Cinco projéteis em leque |

## Inimigos e interações

- Salas 1 e 8 são seguras. Dimensões (largura × altura), por sala: 1: 1120×768; 2: 1280×800; 3: 1440×864; 4: 1280×960; 5: 1536×896; 6: 1408×960; 7: 1600×1024; 8: 960×640; 9: 1152×768; 10: 1280×864; 11: 1376×800; 12: 1664×1088.
- Três inimigos: perseguidor corpo a corpo, atirador e patrulheiro rápido. Vagueiam fora do alcance de percepção e perseguem ou atacam quando percebem o jogador.
- Usar grade de navegação e busca em largura para contornar obstáculos; atualizar caminhos periodicamente.
- Percepção dos inimigos: 540 unidades; recálculo de caminhos a cada 0,35 s quando há obstáculo. Atirador: aproxima-se do jogador e continua andando enquanto dispara, respeitando obstáculos; velocidade 90, alcance 360, disparos de 10 de dano a cada 1,4 s, projéteis a 300 unidades/s por até 3 s. Patrulheiro: velocidade 180 e contato de 10 de dano a cada 0,55 s. Os três tipos comuns têm 40 de vida e concedem 20 XP.
- Itens: kit de vida, chave, armas, escudo temporário e bônus temporário de dano.
- Kits recuperam 30 de vida e permanecem no chão quando a vida está cheia. Escudos absorvem 30 de dano; bônus de dano concede 25% por 10 segundos. Recoletas renovam efeitos sem acumular.
- Coleta por contato, exceto armas, que exigem `E`. Uma interação recolhe somente a arma diferente mais próxima, dentro de 44 unidades, equipa a nova e deixa a substituída na posição da coleta. Kits com vida cheia e armas iguais às já carregadas permanecem no chão.
- Caixas têm 40 de vida. Espinhos nas salas 4–7 descansam por 2 s, avisam por 1 s e ficam ativos por 0,8 s; causam 15 de dano com intervalo de 0,6 s a jogador e inimigos. Cada alavanca desativa o grupo da própria sala.
- Interação com NPCs e alavancas exige proximidade de até 60 unidades. Diálogos avançam uma fala por acionamento de E/Enter/Esc, pausando o mundo. Ao concluir a conversa com o curandeiro, a cura de uso único é aplicada e abre-se o atalho bidirecional 8–4.
- Ataques de jogador e inimigos destroem caixas, liberando passagem. Alavancas desativam armadilhas; conversar com o curandeiro abre um atalho. Preservar essas alterações.
- Chefe: 400 de vida, velocidade 95, contato e projéteis de 20 de dano. Alterna perseguição de 1,5 s, aviso de 0,9 s, investida de 0,45 s a 620 unidades/s ou sete tiros num leque de 1,2 radiano a 280 unidades/s; recupera-se por 0,8 s. Abaixo de metade da vida, perseguição cai para 0,9 s, aviso para 0,65 s, recuperação para 0,4 s e tiros sobem para 340 unidades/s. A vitória só é registrada pela morte do chefe da sala 12.

## História, progressão e pontuação

Um explorador entra numa fortaleza para interromper a corrupção de seu guardião. O vigia apresenta o objetivo; o curandeiro explica a corrupção e oferece uma cura completa uma vez por partida.

- Começar com 60 de vida, velocidade de 240 unidades por segundo e nível 1.
- Inimigos comuns concedem 20 XP; chefe concede 100 XP.
- Cada nível exige `50 × nível atual` XP, consumidos na subida, preservando o excedente.
- Cada nível concede um ponto de atributo, distribuído em uma tela que pausa a partida. A tela abre ao subir; pontos podem ser guardados e gastos depois usando `Tab`:
  - **Força:** +10% do dano base por ponto; o bônus temporário multiplica o resultado.
  - **Vitalidade:** +20 de vida máxima e recuperação de 20 de vida.
  - **Agilidade:** +5% da velocidade base por ponto.
- Pontuação: `máximo(0, 100 × inimigos derrotados + 50 × itens coletados + 200 × salas descobertas + 1000 × vitória − piso(distância percorrida / 100) − 2 × vida recuperada)`.
- Contabilizar distância real, incluindo dash, e cura efetiva, incluindo NPC e Vitalidade.
- Exibir vida, escudo, XP, nível, atributos pendentes, armas, chave e pontuação. Mostrar resultado e pontuação final na vitória ou derrota.

## Organização técnica

Priorizar composição, sem hierarquias de herança nesta versão.

- **`Game`:** controla estados, partida, transições e coordenação dos sistemas.
- **`Player`:** mantém estado do jogador, movimento, dash, combate e progressão.
- **`Enemy`:** mantém estado e comportamento, diferenciados por um `enum` de tipo.
- **`Room`:** reúne cenário, entidades, conexões e estado persistente de exploração.
- **Structs:** representar configurações de armas, projéteis, itens, atributos e conexões entre salas.

Separar leitura de entrada, atualização e desenho. Manter campos que exigem invariantes privados e disponibilizar métodos pequenos com responsabilidades claras. Usar funções auxiliares para colisões, navegação e cálculo de pontuação, sem transformar toda operação em uma classe.

Usar `std::vector` para coleções e identificadores para relações entre salas. Passar dados por referência e `const` referência quando adequado. Usar ponteiros sem propriedade para resultados opcionais de buscas, sem mantê-los após alterações que possam invalidá-los. Evitar `new` e `delete` manuais.

Centralizar valores de balanceamento. Manter o build existente e validar com `make`.

## Etapas de implementação

1. **Base:** janela, menu, estados, movimento, paredes e pausa.
2. **Combate:** espada, pistola, dash, um inimigo, dano, morte e reinício.
3. **Exploração:** salas conectadas, persistência, escuridão, chave e portas.
4. **Progressão:** itens, XP, atributos, NPCs e cenário interativo.
5. **Conteúdo:** completar 12 salas, quatro armas, três inimigos e chefe.
6. **Entrega:** instruções de controles, itens e pontuação; acabamento visual; balanceamento e revisão do PDF.

## Validação

- Movimento diagonal, colisões em cantos e dash em diferentes taxas de quadros.
- Dano único por golpe, projéteis e duração da invulnerabilidade.
- Navegação dos inimigos e surgimentos válidos.
- Retorno entre salas com persistência e descoberta do mapa.
- Chave, armas, cura, efeitos temporários, XP excedente e atributos.
- Alterações no cenário causadas por jogador, inimigos e NPCs.
- Pontuação, vitória, derrota e reinício sem estados residuais.
- Atendimento a cada critério do PDF, incluindo structs, referências, ponteiros e segmentação em sub-rotinas.

## Premissas

O alvo inicial será desktop Linux com teclado e mouse. Os valores numéricos são parâmetros iniciais ajustáveis. Salvamento, multiplayer e áudio ficam fora desta versão.
