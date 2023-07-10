#include "world.h"

#include "geom.h"

#define COLOR_PLATFORM ((Vec4f){0.2125f, 0.3f, 0.425f, 1.0f})
#define COLOR_WALL     ((Vec4f){0.1875f, 0.2f, 0.3f, 0.675f})

#define WORLD_CUBES(i) CUBES[OFFSET_WORLD + (i)]

#define CAP_WORLD (CAP_CUBES - (CAP_ENEMIES + CAP_PLAYER))

extern Geom CUBES[CAP_CUBES];
extern Box  BOXES[CAP_CUBES];

extern u32 LEN_CUBES;
extern u32 LEN_WORLD;

#define PLATFORM(x, z, width, depth)   \
    ((Geom){                           \
        .translate = {x, -1.0f, z},    \
        .scale = {width, 1.0f, depth}, \
        .color = COLOR_PLATFORM,       \
    })

#define WALL(x, z, width, depth)       \
    ((Geom){                           \
        .translate = {x, 0.5f, z},     \
        .scale = {width, 2.5f, depth}, \
        .color = COLOR_WALL,           \
    })

void world_init(void) {
    LEN_WORLD = 0;

    WORLD_CUBES(LEN_WORLD++) = PLATFORM(0.0f, -5.0f, 30.0f, 40.0f);
    WORLD_CUBES(LEN_WORLD++) = PLATFORM(16.0f, 0.0f, 20.0f, 25.0f);

    WORLD_CUBES(LEN_WORLD++) = WALL(-7.0f, 5.5f, 10.0f, 14.0f);
    WORLD_CUBES(LEN_WORLD++) = WALL(-7.0f, -14.5f, 10.0f, 15.0f);
    WORLD_CUBES(LEN_WORLD++) = WALL(7.5f, 4.0f, 8.0f, 17.0f);

    WORLD_CUBES(LEN_WORLD++) = WALL(12.0f, -22.0f, 2.0f, 2.0f);
    WORLD_CUBES(LEN_WORLD++) = WALL(4.0f, -22.0f, 2.0f, 2.0f);
    WORLD_CUBES(LEN_WORLD++) = WALL(12.0f, -16.0f, 2.0f, 2.0f);
    WORLD_CUBES(LEN_WORLD++) = WALL(4.0f, -16.0f, 2.0f, 2.0f);
    WORLD_CUBES(LEN_WORLD++) = WALL(12.0f, -10.0f, 2.0f, 2.0f);
    WORLD_CUBES(LEN_WORLD++) = WALL(4.0f, -10.0f, 2.0f, 2.0f);

    WORLD_CUBES(LEN_WORLD++) = WALL(16.0f, -6.0f, 2.0f, 2.0f);
    WORLD_CUBES(LEN_WORLD++) = WALL(22.0f, -6.0f, 2.0f, 2.0f);

    WORLD_CUBES(LEN_WORLD++) = WALL(16.0f, 1.0f, 2.0f, 2.0f);
    WORLD_CUBES(LEN_WORLD++) = WALL(22.0f, 1.0f, 2.0f, 2.0f);

    WORLD_CUBES(LEN_WORLD++) = WALL(16.0f, 8.0f, 2.0f, 2.0f);
    WORLD_CUBES(LEN_WORLD++) = WALL(22.0f, 8.0f, 2.0f, 2.0f);

    EXIT_IF(CAP_WORLD < LEN_WORLD);

    LEN_CUBES = CAP_PLAYER + CAP_ENEMIES + LEN_WORLD;
    EXIT_IF(CAP_CUBES < LEN_CUBES);

    for (u32 i = 0; i < LEN_WORLD; ++i) {
        BOXES[OFFSET_WORLD + i] = geom_box(&CUBES[OFFSET_WORLD + i]);
    }
}
