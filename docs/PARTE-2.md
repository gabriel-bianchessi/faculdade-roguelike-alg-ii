# Parte 2 — requisitos e regras

Fonte: `T3 - Roguelike - 2026-part2.pdf`. Checklist executável em [TODO-parte-2.md](../TODO-parte-2.md).

| Critério funcional (0,4 cada) | Implementação |
|---|---|
| Três dificuldades | Seleção antes de começar; dificuldade identificada na partida e ranking |
| Ao menos três fases | 12 salas originais, com três etapas identificadas e layouts/encontros distintos; tesouro opcional como sala 13 |
| Tempo na tela e pontuação | Tempo ativo no HUD; desconto de um ponto por dez segundos completos |
| Arquivo e ordem da melhor pontuação | Ranking por usuário, ordem decrescente, páginas de oito; desempate por menor tempo |
| Sete itens diferentes | Kit, escudo, fúria, espada, pistola, lança, escopeta, cristal de XP; chave adicional |
| Dificuldade muda probabilidades | Quantidades e tipos de inimigos e chances de recursos diferentes por dificuldade |
| Som | Pendente, adiado pelo usuário |
| Itens alteram visual do personagem | Armas equipadas, contorno de escudo, cor e aura de fúria |
| Interface mostra itens obtidos | Duas armas no HUD; I abre histórico de tipos e efeitos atuais |
| Inimigos pegam ou destroem itens | Consumíveis destruídos por contato, com aviso visual; armas/chave preservadas |
| Companheiro | Vigia recrutável, segue com colisão/BFS e auxilia o combate |
| Aleatoriedade de itens/inimigos | Sorteio semeado de posições, ocupação e tipos, com regras de dificuldade |
| Fases bônus ou segredos | Passagem examinável no depósito para tesouro persistente |
| Farm de experiência | Combate/ondas concedem XP e atributos; cristais são fonte adicional, finita por partida |
| Pausa | Esc, diálogos, inventário e atributos congelam o mundo e o tempo ativo |

Os requisitos da parte 1 continuam válidos. Seus testes permanecem na suíte, incluindo colisões, exploração persistente, armadilhas, cura única, chave, chefe, vitória/derrota e pontuação final.

## Valores das dificuldades

| Regra | Fácil | Normal | Difícil |
|---|---:|---:|---:|
| Ajuste na quantidade base por grupo | -1 | 0 | +1 |
| Chance de consumível nos candidatos restantes | 75% | 50% | 25% |
| Chance de perseguidor | 70% | 34% | 20% |
| Chance de atirador | 20% | 33% | 40% |
| Chance de patrulheiro | 10% | 33% | 40% |

A sala 2 mantém perseguidores para introduzir o combate. Grupos posteriores crescem por onda, limitados às posições válidas disponíveis. As probabilidades não alteram a chave, armas garantidas, NPCs, chefe ou salas seguras. As proporções efetivas variam por partida e pelos candidatos disponíveis.

## Ranking

Uma linha por resultado: `pontuação segundos dificuldade vitória`, com dificuldades 0/1/2 e vitória 0/1. Local: `$XDG_DATA_HOME/rogue-like/ranking.txt`; sem XDG, `$HOME/.local/share/rogue-like/ranking.txt`; sem ambos, `ranking.txt` na pasta atual. Linhas inválidas são ignoradas na leitura; arquivo ausente exibe ranking vazio. Falha de gravação aparece na tela de resultado. Partidas abandonadas não são registradas e o progresso não é salvo.

## Companheiro e segredo

Concluir o diálogo inicial recruta o vigia. O aliado é invulnerável, não bloqueia o jogador e não coleta nem destrói itens. Ataca um alvo de cada vez com linha livre; mortes entram no mesmo fluxo de XP/pontuação do jogador. Mantém recrutamento entre salas e é restaurado ao iniciar outra partida.

No depósito (sala 10), aproximar-se da rachadura inferior direita mostra E para examinar. A passagem revela o tesouro (13), com cristal de 100 XP, escudo e kit. A descoberta pontua uma vez e os consumíveis não se renovam. Portas continuam bloqueadas durante combate. O segredo é opcional; o chefe continua na sala 12.

## Pendências explícitas

Áudio não implementado nesta etapa, conforme solicitado. O modo Computador/NPC que joga sozinho, avaliado separadamente no PDF, aguarda detalhamento e não está implementado. O companheiro e os controladores de teste não substituem esse requisito. Autoria, envio e defesa individual continuam a cargo da equipe.
