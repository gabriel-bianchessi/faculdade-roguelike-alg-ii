#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
output="${1:-entrega/rogue-like.tar.gz}"
mkdir -p "$(dirname "$output")"
if [[ ! -d build/external/raylib-master/src ]]; then
    echo 'Gere as dependencias em build com ./premake5 gmake antes de empacotar.' >&2
    exit 1
fi
tar --transform='s,^,rogue-like/,' --exclude='*.o' --exclude='*.a' \
    -czf "$output" src tests docs scripts resources sprites-knight.png README.md TODO.md TODO-parte-2.md anotacoes.md \
    'Spritesheet de Cavaleiro Corrompido.png' 'Spritesheet do Besteiro Corrompido.png' \
    'Spritesheet do Patrulheiro Sombrio.png' \
    'Spritesheet do Cavaleiro Sombrio Corrompido.png' 'Spritesheet de Projéteis Mágicos Roxos.png' \
    'Guarda medieval em animação pixel art.png' 'Spritesheet do Curandeiro Medieval.png' \
    'Câmara de Pedra com Quatro Portais.png' \
    'T3 - Roguelike - 2026.pdf' 'T3 - Roguelike - 2026-part2.pdf' build/premake5 build/premake5.lua build/ecc \
    build/external/raylib-master/src build/external/raylib-master/LICENSE
printf 'Pacote criado: %s\n' "$output"
