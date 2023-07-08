#!/usr/bin/env bash

set -eu

flags_shared=(
    "-ferror-limit=1"
    "-march=native"
    -O3
    -Werror
    -Weverything
    -Wno-padded
)
flags_stb=(
    -Wno-cast-align
    -Wno-cast-qual
    -Wno-disabled-macro-expansion
    -Wno-double-promotion
    -Wno-implicit-int-conversion
    -Wno-missing-prototypes
    -Wno-sign-conversion
)
flags_main=(
    -D_DEFAULT_SOURCE
    "-fsanitize=bounds"
    "-fsanitize=float-divide-by-zero"
    "-fsanitize-ignorelist=$WD/scripts/ignorelist.txt"
    "-fsanitize=implicit-conversion"
    "-fsanitize=integer"
    "-fsanitize=nullability"
    "-fsanitize=undefined"
    -fshort-enums
    -g
    -lGL
    -lglfw
    -lm
    "-std=c99"
    -Wno-c2x-extensions
    -Wno-covered-switch-default
    -Wno-declaration-after-statement
)

clang-format -i -verbose "$WD/src/"*

if [ ! -f "$WD/build/image.o" ]; then
    mold -run clang "${flags_shared[@]}" "${flags_stb[@]}" "-I$WD/lib" -c \
        -o "$WD/build/image.o" "$WD/src/image.c"
fi
mold -run clang "${flags_shared[@]}" "${flags_main[@]}" -o "$WD/bin/main" \
    "$WD/build/image.o" "$WD/src/main.c"
prime-run "$WD/bin/main"
