#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
make -j2
check_dir=$(mktemp -d /tmp/rogue-like-tests.XXXXXX)
trap 'rm -rf "$check_dir"' EXIT
flags=(-std=c++20 -Wall -Wextra -Wshadow -fno-access-control -Isrc -isystem build/external/raylib-master/src)
if [[ "${1:-}" == "--sanitize" ]]; then
    flags+=(-fsanitize=address,undefined -fno-omit-frame-pointer -g)
fi
objects=()
for source in src/*.cpp; do
    [[ "$source" == src/main.cpp ]] && continue
    object="$check_dir/$(basename "${source%.cpp}").o"
    g++ "${flags[@]}" -c "$source" -o "$object"
    objects+=("$object")
done
for test in tests/*.cpp; do
    binary="$check_dir/$(basename "${test%.cpp}")"
    g++ "${flags[@]}" "$test" "${objects[@]}" bin/Debug/libraylib.a -lX11 -lpthread -lm -ldl -lrt -o "$binary"
    "$binary"
done
