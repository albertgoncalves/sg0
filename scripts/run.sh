#!/usr/bin/env bash

set -eu

    # -D_POSIX_C_SOURCE
    # "-fsanitize=address"
    # "-fsanitize=bounds"
    # "-fsanitize=float-divide-by-zero"
    # "-fsanitize=implicit-conversion"
    # "-fsanitize=integer"
    # "-fsanitize=nullability"
    # "-fsanitize=undefined"
flags=(
    -D_DEFAULT_SOURCE
    "-ferror-limit=1"
    -fshort-enums
    -lGL
    -lglfw
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
"$WD/bin/main" "$WD/src/vert.glsl" "$WD/src/frag.glsl"
