#include "world.h"

#include "geom.h"

#define COLOR_PLATFORM ((Vec4f){0.2125f, 0.3f, 0.425f, 1.0f})
#define COLOR_WALL     ((Vec4f){0.1875f, 0.2f, 0.3f, 0.675f})

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

static const Vec4f PLATFORMS[] = {
    {0.0f, -5.0f, 30.0f, 40.0f},
    {20.5f, 0.0f, 11.0f, 25.0f},
};
#define LEN_PLATFORMS (sizeof(PLATFORMS) / sizeof(PLATFORMS[0]))

static const Vec4f WALLS[] = {
    {-7.0f, 5.5f, 10.0f, 14.0f},
    {-7.0f, -14.5f, 10.0f, 15.0f},
    {7.5f, 4.0f, 8.0f, 17.0f},
    {12.0f, -22.0f, 2.0f, 2.0f},
    {4.0f, -22.0f, 2.0f, 2.0f},
    {12.0f, -16.0f, 2.0f, 2.0f},
    {4.0f, -16.0f, 2.0f, 2.0f},
    {12.0f, -10.0f, 2.0f, 2.0f},
    {4.0f, -10.0f, 2.0f, 2.0f},
    {16.0f, -6.0f, 2.0f, 2.0f},
    {22.0f, -6.0f, 2.0f, 2.0f},
    {16.0f, 1.0f, 2.0f, 2.0f},
    {22.0f, 1.0f, 2.0f, 2.0f},
    {16.0f, 8.0f, 2.0f, 2.0f},
    {22.0f, 8.0f, 2.0f, 2.0f},
};
#define LEN_WALLS (sizeof(WALLS) / sizeof(WALLS[0]))

void world_init(void) {
    LEN_WORLD = LEN_PLATFORMS + LEN_WALLS;
    EXIT_IF(CAP_WORLD < LEN_WORLD);

    for (u32 i = 0; i < LEN_PLATFORMS; ++i) {
        CUBES[i] = PLATFORM(PLATFORMS[i].x, PLATFORMS[i].y, PLATFORMS[i].z, PLATFORMS[i].w);
        BOXES[i] = geom_box(&CUBES[i]);
    }

    for (u32 i = 0; i < LEN_WALLS; ++i) {
        CUBES[LEN_PLATFORMS + i] = WALL(WALLS[i].x, WALLS[i].y, WALLS[i].z, WALLS[i].w);
        BOXES[LEN_PLATFORMS + i] = geom_box(&CUBES[LEN_PLATFORMS + i]);
    }

    for (u32 i = 0; i < LEN_WORLD; ++i) {
        const Geom cube = CUBES[i];
        const Box  box = BOXES[i];

        u32 j = i;
        for (; (0 < j) && (box.left_bottom_back.x < BOXES[j - 1].left_bottom_back.x); --j) {
            CUBES[j] = CUBES[j - 1];
            BOXES[j] = BOXES[j - 1];
        }
        CUBES[j] = cube;
        BOXES[j] = box;
    }

    OFFSET_PLAYER = LEN_WORLD;
    OFFSET_ENEMIES = LEN_WORLD + CAP_PLAYER;
}
