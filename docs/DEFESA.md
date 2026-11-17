# Preparação para a defesa

Cada integrante deve ensaiar sozinho. Este roteiro prepara o ensaio; não registra que a equipe já o realizou. O PDF prevê perguntas a qualquer integrante e uma alteração individual sem auxílio dos colegas.

## Roteiro de demonstração

1. Compile com `make` e inicie `./bin/Debug/rogue-like`.
2. Abra as três páginas das instruções. Explique o objetivo e a fórmula da pontuação.
3. Comece uma partida. Mostre movimento diagonal, mira, colisões e dash; fale com o vigia.
4. Entre na sala 2 e enfrente um inimigo usando Q para comparar as armas.
5. Mostre a memória do mapa ao voltar à sala 1; um inimigo morto não reaparece.
6. Ao subir de nível, explique consumo do XP, excedente e atributos. Saia com pontos guardados e reabra com Tab.
7. Visite a sala 8, converse e mostre o atalho até 4. Explique a cura de uso único.
8. Em 4/5, destrua uma caixa e desligue espinhos pela alavanca. Volte para provar persistência.
9. Encontre a chave em 9. Recolha lança em 10 e escopeta em 11, mostrando a arma descartada.
10. Entre em 12 e identifique os avisos do chefe. Mostre pontuação final e reinício após vitória ou derrota.

## Fluxo do código

`main.cpp` cria `Game`. `run` separa entrada, atualização e desenho. `GameState` impede que combate e relógios avancem em menus, diálogos e atributos. O mundo atualiza em passos de até 1/240 s, preservando colisões e intervalos mesmo em quadros mais lentos.

`Player` protege vida, timers, XP e pontos. `Room::move` resolve deslocamento por eixo até a face sólida mais próxima; não depende só da posição final. `Dungeon` guarda as 12 salas por valor e muda apenas o índice atual. As conexões usam IDs; uma busca pode retornar um ponteiro opcional sem transferir propriedade.

`Combat` guarda armas, golpes e projéteis. O dano do golpe/tiro é fotografado ao atacar. O inimigo registra o identificador do golpe para receber dano uma vez. Projéteis usam o primeiro contato ao longo do segmento; removemos mortos após resolver os tiros e concedemos XP nesse ponto único.

`Enemy` usa enum para escolher comportamento, sem herança. A busca em largura visita células transitáveis com uma fila e reconstrói o caminho por predecessores. Caminhos são recalculados por intervalo. Destruir uma caixa reconstrói os sólidos e a transitabilidade sem apagar a memória de exploração.

`Attributes`, `WeaponConfig`, `Projectile`, `Item`, `Connection`, `Cell`, `Box`, `Spikes`, `Lever` e `Npc` são structs de dados. Parâmetros mutáveis usam referência; leitura usa `const` referência. `nearbyWeapon` retorna ponteiro sem propriedade; `collectItems` copia os dados necessários antes de alterar o vetor. Não há alocação/desalocação manual.

## Perguntas para cada integrante responder

- Por que normalizar a diagonal e multiplicar por tempo?
- Por que um tiro rápido precisa de colisão no segmento?
- Como evitamos que escudos, bônus e invulnerabilidades se somem indevidamente?
- Como o excedente de XP permite várias subidas em uma chamada?
- O que conta como coleta, cura, distância e descoberta para a pontuação?
- Por que retornar à sala preserva os inimigos mortos?
- Quando um ponteiro para um elemento de `vector` deixa de ser válido?
- Por que a remoção de inimigos ocorre depois da iteração dos projéteis?
- Como o BFS encerra quando não há caminho?
- Como provar que reiniciar limpa NPC, chefe, estatísticas e exploração?

## Exercícios individuais

Faça cada exercício numa cópia ou branch de treino. Compile, explique o efeito e restaure a regra original. Alguns testes de valores conhecidos devem falhar após alterar o balanceamento; atualizá-los requer justificar a nova regra, não apenas apagar a asserção.

| Exercício | Onde começar | Como conferir |
|---|---|---|
| Aumentar vida inicial de 60 para 80 | `Config::initialHealth` | HUD e reinício; máximo e cura não ultrapassam 80 |
| Mudar Agilidade para 8% | `Config::agilityBonus` | Um ponto deve produzir velocidade 259,2 a partir de 240 |
| Dar 12 de dano à pistola | `pistol` em `Weapon.h` | Um tiro deixa um perseguidor com 28 de vida |
| Reduzir intervalo do patrulheiro | `Config::patrolInterval` | Ataques respeitam o novo intervalo e a proteção do jogador |
| Mudar recompensa por coleta para 75 | `Config::scoreItem` | Uma coleta aumenta o componente de itens em 75 |
| Alterar custo de nível para 60 × nível | `Config::levelCost` | 180 XP consomem 60 + 120, chegando ao nível 3 sem excedente |
| Adicionar um kit opcional | `Dungeon`/`populate` | Posição livre, coleta efetiva e persistência após retornar |

Depois, rode `bash tests/run.sh` com os valores originais. O ensaio e a execução individual destes exercícios pelos integrantes continuam como tarefas humanas na TODO.
