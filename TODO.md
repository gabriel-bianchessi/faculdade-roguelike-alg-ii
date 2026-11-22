# Checklist de implementação

Baseado no [plano ampliado](anotacoes.md). Executar na ordem das etapas e marcar `[x]` somente após implementar e verificar a tarefa. Os valores de balanceamento iniciais estão no plano.

## Correções e melhorias solicitadas

- [x] Corrigir movimento acumulado ao pressionar paredes; verificar saída imediata na direção oposta.
- [x] Adicionar barras de vida e recuperação do dash no canto inferior esquerdo.
- [x] Adicionar painel da arma atual no canto inferior direito e pentes de 15 tiros (pistola) e 7 (escopeta), com reserva infinita e recarga.
- [x] Recortar e integrar os sprites animados do vigia e do curandeiro.
- [x] Executar testes de regressão e conferir a interface em janela maximizada.

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
- [x] Inicializar jogador com 60 de vida e velocidade de 240 unidades por segundo.
- [x] Criar `Room` e uma sala de teste com chão, paredes e obstáculos.
- [x] Definir coordenadas do mundo e converter a posição do mouse para esse espaço ao mirar.
- [x] Implementar movimento WASD usando tempo entre quadros e normalização diagonal.
- [x] Implementar colisão com paredes e obstáculos, permitindo deslizar ao longo das paredes.
- [x] Desenhar jogador, cenário e indicador da direção da mira com formas geométricas.
- [x] Ajustar câmera ou escala de visualização para acomodar salas de tamanhos diferentes.
- [x] Verificar movimento nos quatro sentidos e diagonais, cantos e limites da sala.
- [x] Comparar movimento em diferentes taxas de quadros.

## 3. Primeiro combate jogável

- [x] Criar structs de configuração de arma e projétil; identificar categorias e tipos com enums.
- [x] Equipar espada e pistola iniciais; implementar troca com `Q` e ataque com botão esquerdo.
- [x] Implementar alcance, área e intervalo da espada; registrar alvos atingidos para não repetir dano no mesmo golpe.
- [x] Implementar disparo da pistola com direção, velocidade, dano e intervalo entre tiros.
- [x] Remover projéteis ao atingir parede ou alvo e ao ultrapassar sua duração máxima.
- [x] Criar `Enemy` com posição, vida, velocidade e tipo; começar com o perseguidor.
- [x] Implementar dano e morte de inimigos, removendo entidades sem invalidar a iteração dos vetores.
- [x] Implementar ataque do perseguidor com intervalo entre danos, evitando dano a cada quadro.
- [x] Implementar dano ao jogador e invulnerabilidade de 0,5 segundo após receber dano.
- [x] Implementar dash de 0,24 segundo, recarga de 1 segundo e invulnerabilidade nos primeiros 0,12 segundo.
- [x] Impedir dash através de paredes, inclusive com deslocamentos maiores entre quadros.
- [x] Impedir soma indevida dos tempos de invulnerabilidade de dano e dash.
- [x] Implementar derrota quando a vida chegar a zero e opção de reinício.
- [x] Exibir vida, arma equipada e disponibilidade do dash.
- [x] Verificar dano único por golpe, cadência, colisão dos tiros, invulnerabilidade e reinício após morte.

## 4. Conexões, exploração e persistência

- [x] Criar struct de conexão com identificadores das salas, porta e posição de chegada.
- [x] Montar primeiro duas salas conectadas e testar ida e volta.
- [x] Manter o estado de todas as salas durante a partida, sem recriar inimigos e itens a cada visita.
- [x] Posicionar o jogador em uma área livre ao entrar e impedir transições repetidas imediatamente.
- [x] Remover projéteis da sala ao sair, evitando disparos residuais no retorno.
- [x] Criar a grade de células usada pelo mapa e pela navegação.
- [x] Implementar máscara de escuridão, revelação próxima ao jogador e memória das células exploradas.
- [x] Registrar a primeira descoberta de cada sala para a pontuação.
- [x] Criar chave coletável e porta trancada entre as salas 7 e 12.
- [x] Exibir indicação de porta trancada e de posse da chave.
- [x] Verificar retorno às salas com inimigos mortos, itens removidos e exploração preservada.

## 5. Itens e arsenal completo

- [x] Criar struct `Item` com tipo, posição e dados necessários ao efeito.
- [x] Implementar coleta por contato para itens comuns e por `E` para armas.
- [x] Implementar kit que recupera até 30 de vida; mantê-lo no chão quando a vida estiver cheia.
- [x] Implementar escudo que absorve 30 de dano antes de descontar a vida.
- [x] Implementar bônus de 25% de dano por 10 segundos, suspendendo o temporizador durante pausas.
- [x] Impedir acúmulo de escudos e bônus; recoletas renovam o efeito correspondente.
- [x] Implementar lança com golpe longo e estreito conforme os valores do plano.
- [x] Implementar escopeta com cinco projéteis em leque conforme os valores do plano.
- [x] Limitar o inventário a uma arma de cada categoria e deixar a substituída no chão.
- [x] Impedir que uma única interação recolha imediatamente a arma recém-descartada.
- [x] Registrar cada coleta efetiva para a pontuação.
- [x] Verificar cura parcial, vida cheia, absorção de dano, expiração do bônus e substituição de armas.

## 6. XP, atributos e pontuação

- [x] Criar struct de atributos e iniciar jogador no nível 1, sem XP e sem pontos disponíveis.
- [x] Conceder 20 XP por inimigo comum derrotado e 100 XP pelo chefe, uma única vez por morte.
- [x] Aplicar custo de subida de `50 × nível atual`, preservando excedente e permitindo múltiplas subidas.
- [x] Conceder um ponto por nível e criar tela de distribuição que pausa o mundo.
- [x] Aplicar Força: +10% do dano base por ponto.
- [x] Aplicar Vitalidade: +20 de vida máxima e recuperação de até 20 de vida por ponto.
- [x] Aplicar Agilidade: +5% da velocidade base por ponto.
- [x] Recalcular atributos a partir das bases, sem acumular o mesmo modificador a cada atualização.
- [x] Registrar inimigos derrotados, itens coletados, salas descobertas, distância real e vida efetivamente recuperada.
- [x] Incluir dash na distância e curas de kits, NPC e Vitalidade na recuperação registrada.
- [x] Implementar a fórmula de pontuação de `anotacoes.md`, com mínimo zero e bônus de vitória aplicado uma única vez.
- [x] Exibir vida, escudo, XP, nível, pontos pendentes, armas, chave e pontuação no HUD.
- [x] Conferir a fórmula com valores conhecidos e verificar múltiplas subidas e gastos de pontos.

## 7. Cenário interativo, NPCs e história

- [x] Implementar caixas com vida, bloqueio de movimento e destruição por ataques de jogador e inimigos.
- [x] Atualizar colisões e grade de navegação quando uma caixa for destruída ou uma passagem aberta.
- [x] Implementar espinhos com estados de aviso, ativação e descanso.
- [x] Aplicar dano de armadilha ao jogador e aos inimigos, com intervalo para evitar dano a cada quadro.
- [x] Implementar alavancas acionadas por `E` que desativam armadilhas associadas.
- [x] Criar interação por proximidade e indicação visual para NPCs e mecanismos.
- [x] Adicionar o vigia na sala 1 com diálogo de apresentação do objetivo.
- [x] Adicionar o curandeiro na sala 8 com explicação da corrupção e cura completa uma vez por partida.
- [x] Conectar o atalho aberto pelo diálogo às salas existentes e registrar sua abertura de forma persistente.
- [x] Exibir diálogos pausando o combate e impedir que a mesma tecla avance várias falas instantaneamente.
- [x] Verificar que caixas, alavancas, cura usada e atalho continuam alterados após sair e voltar.

## 8. Inimigos completos e surgimento aleatório

- [x] Implementar busca em largura sobre a grade transitável para contornar obstáculos.
- [x] Recalcular caminhos em intervalos, considerando mudanças nas passagens.
- [x] Implementar vaguear entre posições válidas fora do alcance de percepção.
- [x] Implementar perseguidor com aproximação e ataque corpo a corpo.
- [x] Implementar atirador com alcance de disparo e tiros bloqueados por paredes.
- [x] Implementar patrulheiro rápido com movimentação e ritmo de ataque próprios.
- [x] Manter colisões físicas de todos os inimigos com paredes e obstáculos.
- [x] Definir posições candidatas para inimigos e itens em cada sala.
- [x] Sortear ocupação somente no início da partida, rejeitando sobreposições e entradas bloqueadas.
- [x] Impedir surgimento de inimigos na sala segura.
- [x] Verificar caminhos sem saída, obstáculos destruídos e várias entidades ativas sem engasgos.

## 9. Montagem das 12 salas

- [x] Montar o caminho principal `1 → 2 → 3 → 4 → 5 → 6 → 7 → 12`.
- [x] Montar as ramificações `2 → 8`, `3 → 9`, `5 → 10` e `6 → 11`.
- [x] Finalizar sala 1 com entrada segura e vigia.
- [x] Finalizar salas 2 e 3 com introdução gradual de inimigos e exploração.
- [x] Finalizar salas 4 e 5 com obstáculos, armadilhas e mecanismos.
- [x] Finalizar salas 6 e 7 com combinações mais difíceis e acesso trancado ao chefe.
- [x] Finalizar sala 8 como área segura com curandeiro.
- [x] Finalizar sala 9 com a chave obrigatória e caminho acessível antes da porta trancada.
- [x] Finalizar salas 10 e 11 com lança, escopeta e recursos opcionais.
- [x] Finalizar sala 12 com espaço para reconhecer e evitar os ataques do chefe.
- [x] Variar dimensões e disposição de obstáculos sem perder consistência visual.
- [x] Percorrer todas as conexões e confirmar que a masmorra pode ser concluída sem bloqueios de progressão.

## 10. Chefe e encerramento da partida

- [x] Implementar chefe com vida, estados de ataque e colisões próprias.
- [x] Alternar perseguição, investida com aviso visual e disparos em leque.
- [x] Reduzir intervalos entre ataques abaixo de metade da vida.
- [x] Sinalizar perigos com tempo suficiente para reação e esquiva.
- [x] Ativar vitória somente ao derrotar o chefe na sala final.
- [x] Criar telas de vitória e derrota com pontuação final e opções de reiniciar ou voltar ao menu.
- [x] Reiniciar salas, inimigos, itens, atributos, exploração, NPCs, temporizadores e estatísticas.
- [x] Completar uma partida vitoriosa e uma derrota, reiniciando após cada uma.

## 11. Acabamento e entrega

- [x] Padronizar cores e silhuetas de jogador, inimigos, itens e perigos.
- [x] Adicionar efeitos visuais de ataque, dano, invulnerabilidade e coleta.
- [x] Garantir texto legível e HUD sem sobreposição com controles ou diálogos.
- [x] Completar menu de ajuda com controles, objetivo, itens, atributos e fórmula de pontuação.
- [x] Ajustar dano, vida, cadência, alcance, velocidade e distribuição de recursos após partidas completas.
- [x] Conferir ausência de piscadas, travamentos e deslocamentos incoerentes.
- [x] Revisar uso de classes, structs, funções pequenas, referências e ponteiros conforme os critérios do PDF.
- [x] Revisar remoções de vetores e referências ou ponteiros que possam ser invalidados.
- [x] Compilar com `make` e corrigir avisos relevantes do projeto.
- [x] Atualizar o README com requisitos, compilação, execução e controles.
- [x] Conferir individualmente todos os critérios de avaliação do PDF.
- [x] Preparar a entrega pelo GitHub ou material didático conforme o enunciado.

Os roteiros para as duas tarefas humanas restantes estão em [docs/DEFESA.md](docs/DEFESA.md). As evidências técnicas estão em [docs/VALIDACAO.md](docs/VALIDACAO.md), e a preparação do pacote/envio em [docs/ENTREGA.md](docs/ENTREGA.md). Marcar ensaio e prática somente após cada integrante realizá-los.

## Limites desta versão

Desktop Linux, teclado e mouse. Sem multiplayer, salvamento, geração procedural ou áudio. A checklist detalha a implementação; mudanças nas regras devem ser refletidas também em `anotacoes.md`.
