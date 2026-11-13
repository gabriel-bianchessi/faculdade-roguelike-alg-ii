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
- Criar salas de dimensões diferentes com paredes sólidas, caixas destrutíveis, portas, alavancas e armadilhas.
- Câmera acompanha o jogador com zoom fixo de 1,2×, limitada às bordas da sala; não é necessário mostrar a sala inteira.
- Revelar células de 32 unidades num raio de 240 unidades do jogador e manter as regiões exploradas escurecidas. Inimigos fora da área visível ficam ocultos.
- Caminhar sobre uma porta numerada troca de sala. As chegadas ficam fora do gatilho da porta, com intervalo mínimo de 0,3 segundo entre transições; o dash é interrompido ao entrar. A chave não é consumida ao atravessar a porta 7–12.
- Sortear inimigos e itens entre posições válidas predefinidas no início da partida, sem bloquear entradas.

## Controles e combate

- `WASD`: movimentar; mouse: mirar; botão esquerdo: atacar.
- `Q`: alternar entre as duas armas; `Espaço`: dash; `E`: interagir ou coletar armas; `Esc`: pausar.
- Normalizar movimento diagonal e controlar velocidades e temporizadores pelo tempo entre quadros.
- Dash de 0,24 segundo, invulnerável na primeira metade, com recarga de 1 segundo e colisão com paredes.
- Após sofrer dano, conceder 0,5 segundo de invulnerabilidade. Não somar esse tempo ao do dash.
- Espada: alcance de 76 unidades, largura de 64 e golpe ativo por 0,12 segundo. Pistola: projéteis a 650 unidades por segundo, raio de 4 e duração máxima de 2 segundos.
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

- Três inimigos: perseguidor corpo a corpo, atirador e patrulheiro rápido. Vagueiam fora do alcance de percepção e perseguem ou atacam quando percebem o jogador.
- Usar grade de navegação e busca em largura para contornar obstáculos; atualizar caminhos periodicamente.
- Itens: kit de vida, chave, armas, escudo temporário e bônus temporário de dano.
- Kits recuperam 30 de vida e permanecem no chão quando a vida está cheia. Escudos absorvem 30 de dano; bônus de dano concede 25% por 10 segundos. Recoletas renovam efeitos sem acumular.
- Coleta por contato, exceto armas, que exigem `E`.
- Espinhos alternam aviso, ativação e descanso e atingem jogador e inimigos.
- Ataques de jogador e inimigos destroem caixas, liberando passagem. Alavancas desativam armadilhas; conversar com o curandeiro abre um atalho. Preservar essas alterações.
- Chefe alterna perseguição, investida anunciada e disparos em leque. Abaixo de metade da vida, ataca com intervalos menores.

## História, progressão e pontuação

Um explorador entra numa fortaleza para interromper a corrupção de seu guardião. O vigia apresenta o objetivo; o curandeiro explica a corrupção e oferece uma cura completa uma vez por partida.

- Começar com 100 de vida, velocidade de 240 unidades por segundo e nível 1.
- Inimigos comuns concedem 20 XP; chefe concede 100 XP.
- Cada nível exige `50 × nível atual` XP, consumidos na subida, preservando o excedente.
- Cada nível concede um ponto de atributo, distribuído em uma tela que pausa a partida:
  - **Força:** +10% do dano base por ponto.
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
