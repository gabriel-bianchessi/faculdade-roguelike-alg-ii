# Parte 2 — checklist de implementação

Base: `T3 - Roguelike - 2026-part2.pdf`. Marcar somente após implementar e verificar.
O modo Computador/NPC que joga sozinho será detalhado separadamente.

## 1. Dificuldades e probabilidades

- [x] Seleção Fácil, Normal e Difícil antes de iniciar.
- [x] Probabilidades distintas de quantidade/tipos de inimigos e recursos, preservando chave e progressão.
- [x] Identificar dificuldade na partida e verificar os sorteios com sementes reproduzíveis.

## 2. Pontuação e ranking

- [x] Considerar tempo ativo na pontuação, congelando durante pausas e diálogos.
- [x] Salvar resultados de vitória/derrota uma única vez em arquivo.
- [x] Exibir ranking ordenado, com dificuldade, duração e resultado; tratar arquivo ausente ou inválido.

## 3. Itens e interface

- [x] Mostrar as duas armas carregadas e os efeitos/itens obtidos com identificação visual.
- [x] Destacar visualmente o personagem enquanto o bônus de dano estiver ativo.
- [x] Adicionar um oitavo item distinto para não depender da contagem da chave.

## 4. Inimigos e itens

- [x] Inimigos comuns destruírem consumíveis por contato, com indicação visual.
- [x] Preservar armas, chave e demais objetos essenciais à progressão.

## 5. Companheiro

- [x] Recrutar aliado, seguir o jogador com colisão/navegação e acompanhar transições.
- [x] Ajudar no combate, concedendo XP/pontuação pelas mortes uma única vez.
- [x] Congelar na pausa e reiniciar corretamente.

## 6. Fase bônus e apresentação das fases

- [x] Adicionar passagem escondida e sala bônus com recompensa persistente.
- [x] Identificar fases/áreas na interface e preservar a rota até o chefe.

## 7. Validação e documentação

- [x] Testes de regressão e dos requisitos novos; conferência visual.
- [x] Atualizar regras, controles, entrega e evidências da parte 2.

## 8. Áudio — adiado a pedido

- [ ] Efeitos de ataque, dano, coleta, interação e encerramento; controle de volume.

## Fora deste escopo

- [ ] Detalhar separadamente o modo Computador/NPC capaz de jogar e vencer sozinho.
