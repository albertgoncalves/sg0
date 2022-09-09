#!/usr/bin/env bash

set -eu

flags=(
    -D_DEFAULT_SOURCE
    "-ferror-limit=1"
    "-fsanitize=bounds"
    "-fsanitize=float-divide-by-zero"
    "-fsanitize=implicit-conversion"
    "-fsanitize=integer"
    "-fsanitize=nullability"
    "-fsanitize=undefined"
    -g
    -lGL
    -lglfw
    -lm
    "-march=native"
    -O3
    "-std=c99"
    -Werror
    -Weverything
    -Wno-c2x-extensions
    -Wno-declaration-after-statement
    -Wno-padded
    -Wno-reserved-macro-identifier
)

clang-format -i -verbose "$WD/src/"*
mold -run clang "${flags[@]}" -o "$WD/bin/main" "$WD/src/main.c"
prime-run "$WD/bin/main"
