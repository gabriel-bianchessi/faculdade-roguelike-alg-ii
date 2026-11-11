# Checklist de implementação

Baseado no [plano ampliado](anotacoes.md). Executar na ordem das etapas e marcar `[x]` somente após implementar e verificar a tarefa. Os valores de balanceamento iniciais estão no plano.

## 1. Estrutura e fluxo do jogo

- [x] Compilar o template com `make` e confirmar que a janela abre corretamente.
- [x] Centralizar configurações de janela, cores e valores de balanceamento.
- [x] Criar a classe `Game` e separar inicialização, entrada, atualização, desenho e encerramento.
- [x] Criar estados para menu, partida, pausa, distribuição de atributos, vitória e derrota.
- [x] Criar menu com iniciar partida, instruções e sair.
- [x] Implementar pausa com retomada e retorno ao menu; impedir atualização do mundo durante a pausa.
- [x] Tratar `Esc` como pausa durante a partida, evitando o fechamento automático padrão da Raylib.
- [x] Criar rotina única para iniciar e reiniciar uma partida com estado limpo.
- [x] Verificar transições entre menu, partida e pausa, inclusive após reiniciar.

## 2. Jogador, sala inicial e colisões

- [x] Criar `Player` com posição, direção, vida atual, vida máxima e velocidade.
- [x] Inicializar jogador com 100 de vida e velocidade de 240 unidades por segundo.
- [x] Criar `Room` e uma sala de teste com chão, paredes e obstáculos.
- [x] Definir coordenadas do mundo e converter a posição do mouse para esse espaço ao mirar.
- [x] Implementar movimento WASD usando tempo entre quadros e normalização diagonal.
- [x] Implementar colisão com paredes e obstáculos, permitindo deslizar ao longo das paredes.
- [x] Desenhar jogador, cenário e indicador da direção da mira com formas geométricas.
- [x] Ajustar câmera ou escala de visualização para acomodar salas de tamanhos diferentes.
- [x] Verificar movimento nos quatro sentidos e diagonais, cantos e limites da sala.
- [x] Comparar movimento em diferentes taxas de quadros.

## 3. Primeiro combate jogável

- [ ] Criar structs de configuração de arma e projétil; identificar categorias e tipos com enums.
- [ ] Equipar espada e pistola iniciais; implementar troca com `Q` e ataque com botão esquerdo.
- [ ] Implementar alcance, área e intervalo da espada; registrar alvos atingidos para não repetir dano no mesmo golpe.
- [ ] Implementar disparo da pistola com direção, velocidade, dano e intervalo entre tiros.
- [ ] Remover projéteis ao atingir parede ou alvo e ao ultrapassar sua duração máxima.
- [ ] Criar `Enemy` com posição, vida, velocidade e tipo; começar com o perseguidor.
- [ ] Implementar dano e morte de inimigos, removendo entidades sem invalidar a iteração dos vetores.
- [ ] Implementar ataque do perseguidor com intervalo entre danos, evitando dano a cada quadro.
- [ ] Implementar dano ao jogador e invulnerabilidade de 0,5 segundo após receber dano.
- [ ] Implementar dash de 0,24 segundo, recarga de 1 segundo e invulnerabilidade nos primeiros 0,12 segundo.
- [ ] Impedir dash através de paredes, inclusive com deslocamentos maiores entre quadros.
- [ ] Impedir soma indevida dos tempos de invulnerabilidade de dano e dash.
- [ ] Implementar derrota quando a vida chegar a zero e opção de reinício.
- [ ] Exibir vida, arma equipada e disponibilidade do dash.
- [ ] Verificar dano único por golpe, cadência, colisão dos tiros, invulnerabilidade e reinício após morte.

## 4. Conexões, exploração e persistência

- [ ] Criar struct de conexão com identificadores das salas, porta e posição de chegada.
- [ ] Montar primeiro duas salas conectadas e testar ida e volta.
- [ ] Manter o estado de todas as salas durante a partida, sem recriar inimigos e itens a cada visita.
- [ ] Posicionar o jogador em uma área livre ao entrar e impedir transições repetidas imediatamente.
- [ ] Remover projéteis da sala ao sair, evitando disparos residuais no retorno.
- [ ] Criar a grade de células usada pelo mapa e pela navegação.
- [ ] Implementar máscara de escuridão, revelação próxima ao jogador e memória das células exploradas.
- [ ] Registrar a primeira descoberta de cada sala para a pontuação.
- [ ] Criar chave coletável e porta trancada entre as salas 7 e 12.
- [ ] Exibir indicação de porta trancada e de posse da chave.
- [ ] Verificar retorno às salas com inimigos mortos, itens removidos e exploração preservada.

## 5. Itens e arsenal completo

- [ ] Criar struct `Item` com tipo, posição e dados necessários ao efeito.
- [ ] Implementar coleta por contato para itens comuns e por `E` para armas.
- [ ] Implementar kit que recupera até 30 de vida; mantê-lo no chão quando a vida estiver cheia.
- [ ] Implementar escudo que absorve 30 de dano antes de descontar a vida.
- [ ] Implementar bônus de 25% de dano por 10 segundos, suspendendo o temporizador durante pausas.
- [ ] Impedir acúmulo de escudos e bônus; recoletas renovam o efeito correspondente.
- [ ] Implementar lança com golpe longo e estreito conforme os valores do plano.
- [ ] Implementar escopeta com cinco projéteis em leque conforme os valores do plano.
- [ ] Limitar o inventário a uma arma de cada categoria e deixar a substituída no chão.
- [ ] Impedir que uma única interação recolha imediatamente a arma recém-descartada.
- [ ] Registrar cada coleta efetiva para a pontuação.
- [ ] Verificar cura parcial, vida cheia, absorção de dano, expiração do bônus e substituição de armas.

## 6. XP, atributos e pontuação

- [ ] Criar struct de atributos e iniciar jogador no nível 1, sem XP e sem pontos disponíveis.
- [ ] Conceder 20 XP por inimigo comum derrotado e 100 XP pelo chefe, uma única vez por morte.
- [ ] Aplicar custo de subida de `50 × nível atual`, preservando excedente e permitindo múltiplas subidas.
- [ ] Conceder um ponto por nível e criar tela de distribuição que pausa o mundo.
- [ ] Aplicar Força: +10% do dano base por ponto.
- [ ] Aplicar Vitalidade: +20 de vida máxima e recuperação de até 20 de vida por ponto.
- [ ] Aplicar Agilidade: +5% da velocidade base por ponto.
- [ ] Recalcular atributos a partir das bases, sem acumular o mesmo modificador a cada atualização.
- [ ] Registrar inimigos derrotados, itens coletados, salas descobertas, distância real e vida efetivamente recuperada.
- [ ] Incluir dash na distância e curas de kits, NPC e Vitalidade na recuperação registrada.
- [ ] Implementar a fórmula de pontuação de `anotacoes.md`, com mínimo zero e bônus de vitória aplicado uma única vez.
- [ ] Exibir vida, escudo, XP, nível, pontos pendentes, armas, chave e pontuação no HUD.
- [ ] Conferir a fórmula com valores conhecidos e verificar múltiplas subidas e gastos de pontos.

## 7. Cenário interativo, NPCs e história

- [ ] Implementar caixas com vida, bloqueio de movimento e destruição por ataques de jogador e inimigos.
- [ ] Atualizar colisões e grade de navegação quando uma caixa for destruída ou uma passagem aberta.
- [ ] Implementar espinhos com estados de aviso, ativação e descanso.
- [ ] Aplicar dano de armadilha ao jogador e aos inimigos, com intervalo para evitar dano a cada quadro.
- [ ] Implementar alavancas acionadas por `E` que desativam armadilhas associadas.
- [ ] Criar interação por proximidade e indicação visual para NPCs e mecanismos.
- [ ] Adicionar o vigia na sala 1 com diálogo de apresentação do objetivo.
- [ ] Adicionar o curandeiro na sala 8 com explicação da corrupção e cura completa uma vez por partida.
- [ ] Conectar o atalho aberto pelo diálogo às salas existentes e registrar sua abertura de forma persistente.
- [ ] Exibir diálogos pausando o combate e impedir que a mesma tecla avance várias falas instantaneamente.
- [ ] Verificar que caixas, alavancas, cura usada e atalho continuam alterados após sair e voltar.

## 8. Inimigos completos e surgimento aleatório

- [ ] Implementar busca em largura sobre a grade transitável para contornar obstáculos.
- [ ] Recalcular caminhos em intervalos, considerando mudanças nas passagens.
- [ ] Implementar vaguear entre posições válidas fora do alcance de percepção.
- [ ] Implementar perseguidor com aproximação e ataque corpo a corpo.
- [ ] Implementar atirador com alcance de disparo e tiros bloqueados por paredes.
- [ ] Implementar patrulheiro rápido com movimentação e ritmo de ataque próprios.
- [ ] Manter colisões físicas de todos os inimigos com paredes e obstáculos.
- [ ] Definir posições candidatas para inimigos e itens em cada sala.
- [ ] Sortear ocupação somente no início da partida, rejeitando sobreposições e entradas bloqueadas.
- [ ] Impedir surgimento de inimigos na sala segura.
- [ ] Verificar caminhos sem saída, obstáculos destruídos e várias entidades ativas sem engasgos.

## 9. Montagem das 12 salas

- [ ] Montar o caminho principal `1 → 2 → 3 → 4 → 5 → 6 → 7 → 12`.
- [ ] Montar as ramificações `2 → 8`, `3 → 9`, `5 → 10` e `6 → 11`.
- [ ] Finalizar sala 1 com entrada segura e vigia.
- [ ] Finalizar salas 2 e 3 com introdução gradual de inimigos e exploração.
- [ ] Finalizar salas 4 e 5 com obstáculos, armadilhas e mecanismos.
- [ ] Finalizar salas 6 e 7 com combinações mais difíceis e acesso trancado ao chefe.
- [ ] Finalizar sala 8 como área segura com curandeiro.
- [ ] Finalizar sala 9 com a chave obrigatória e caminho acessível antes da porta trancada.
- [ ] Finalizar salas 10 e 11 com lança, escopeta e recursos opcionais.
- [ ] Finalizar sala 12 com espaço para reconhecer e evitar os ataques do chefe.
- [ ] Variar dimensões e disposição de obstáculos sem perder consistência visual.
- [ ] Percorrer todas as conexões e confirmar que a masmorra pode ser concluída sem bloqueios de progressão.

## 10. Chefe e encerramento da partida

- [ ] Implementar chefe com vida, estados de ataque e colisões próprias.
- [ ] Alternar perseguição, investida com aviso visual e disparos em leque.
- [ ] Reduzir intervalos entre ataques abaixo de metade da vida.
- [ ] Sinalizar perigos com tempo suficiente para reação e esquiva.
- [ ] Ativar vitória somente ao derrotar o chefe na sala final.
- [ ] Criar telas de vitória e derrota com pontuação final e opções de reiniciar ou voltar ao menu.
- [ ] Reiniciar salas, inimigos, itens, atributos, exploração, NPCs, temporizadores e estatísticas.
- [ ] Completar uma partida vitoriosa e uma derrota, reiniciando após cada uma.

## 11. Acabamento e entrega

- [ ] Padronizar cores e silhuetas de jogador, inimigos, itens e perigos.
- [ ] Adicionar efeitos visuais de ataque, dano, invulnerabilidade e coleta.
- [ ] Garantir texto legível e HUD sem sobreposição com controles ou diálogos.
- [ ] Completar menu de ajuda com controles, objetivo, itens, atributos e fórmula de pontuação.
- [ ] Ajustar dano, vida, cadência, alcance, velocidade e distribuição de recursos após partidas completas.
- [ ] Conferir ausência de piscadas, travamentos e deslocamentos incoerentes.
- [ ] Revisar uso de classes, structs, funções pequenas, referências e ponteiros conforme os critérios do PDF.
- [ ] Revisar remoções de vetores e referências ou ponteiros que possam ser invalidados.
- [ ] Compilar com `make` e corrigir avisos relevantes do projeto.
- [ ] Atualizar o README com requisitos, compilação, execução e controles.
- [ ] Conferir individualmente todos os critérios de avaliação do PDF.
- [ ] Ensaiar a defesa: cada integrante explica o fluxo, as estruturas e os sistemas implementados.
- [ ] Praticar alterações individuais em atributos, armas, inimigos e regras de pontuação.
- [ ] Preparar a entrega pelo GitHub ou material didático conforme o enunciado.

## Limites desta versão

Desktop Linux, teclado e mouse. Sem multiplayer, salvamento, geração procedural ou áudio. A checklist detalha a implementação; mudanças nas regras devem ser refletidas também em `anotacoes.md`.
