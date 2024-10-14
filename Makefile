MAKEFLAGS += --silent
CC = mold -run clang
FLAGS_ALL = \
	-ferror-limit=1 \
	-march=native \
	-O3 \
	-Werror \
	-Weverything \
	-Wno-padded \
	-Wno-unsafe-buffer-usage
FLAGS_STB = \
	-Wno-cast-align \
	-Wno-cast-qual \
	-Wno-disabled-macro-expansion \
	-Wno-double-promotion \
	-Wno-implicit-int-conversion \
	-Wno-missing-prototypes \
	-Wno-sign-conversion \
	-Wno-switch-default
FLAGS_OBJECTS = \
	-D_DEFAULT_SOURCE \
	-fsanitize=bounds \
	-fsanitize=float-divide-by-zero \
	-fsanitize-ignorelist=scripts/ignorelist.txt \
	-fsanitize=implicit-conversion \
	-fsanitize=integer \
	-fsanitize=nullability \
	-fsanitize=undefined \
	-fshort-enums \
	-g \
	-std=c99 \
	-Wno-c23-extensions \
	-Wno-covered-switch-default \
	-Wno-declaration-after-statement
FLAGS_MAIN = \
	-lGL \
	-lglfw \
	-lm
MODULES = \
	enemy \
	geom \
	graphics \
	math \
	pcg_rng \
	player \
	string \
	time \
	world
OBJECTS = $(foreach x,$(MODULES),build/$(x).o)

.PHONY: all
all: bin/main

.PHONY: clean
clean:
	rm -rf bin/
	rm -rf build/

.PHONY: run
run: all
	./bin/main

.PHONY: profile
profile: all
	./scripts/profile.sh

bin/main: build/image.o $(OBJECTS) src/prelude.h src/main.c
	mkdir -p bin/
	clang-format -i shaders/*.glsl src/prelude.h src/main.c
	$(CC) $(FLAGS_ALL) $(FLAGS_OBJECTS) $(FLAGS_MAIN) -o bin/main \
		build/image.o $(OBJECTS) src/main.c

build/image.o: src/image.h src/image.c
	mkdir -p build/
	clang-format -i src/image.*
	$(CC) $(FLAGS_ALL) $(FLAGS_STB) -c -Ilib -o build/image.o src/image.c

$(OBJECTS): build/%.o: src/%.h src/%.c
	mkdir -p build/
	clang-format -i $^
	$(CC) $(FLAGS_ALL) $(FLAGS_OBJECTS) -c -o $@ $(word 2,$^)
