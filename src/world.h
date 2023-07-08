#ifndef WORLD_H
#define WORLD_H

#include "memory.h"
#include "pcg_rng.h"

#define WORLD_GRAVITY 0.000675f
#define WORLD_FLOOR   -2.5f

#define WORLD_COLOR_PLATFORM ((Vec4f){0.2125f, 0.3f, 0.425f, 1.0f})
#define WORLD_COLOR_WALL     ((Vec4f){0.1875f, 0.2f, 0.3f, 0.675f})

#define WORLD_CUBES(i) CUBES[OFFSET_WORLD + i]

static void init_world(void) {
    (void)get_random_uniform_f32;
    (void)get_random_bounded_u32;
    LEN_WORLD = 8;
    EXIT_IF(CAP_WORLD < LEN_WORLD);
    WORLD_CUBES(0) = (Geom){
        .translate = {0.0f, -1.0f, 0.0f},
        .scale = {20.0f, 1.0f, 20.0f},
        .color = WORLD_COLOR_PLATFORM,
    };
    WORLD_CUBES(1) = (Geom){
        .translate = {25.0f, -1.0f, 0.0f},
        .scale = {20.0f, 1.0f, 20.0f},
        .color = WORLD_COLOR_PLATFORM,
    };
    WORLD_CUBES(2) = (Geom){
        .translate = {12.5f, -1.0f, 0.0f},
        .scale = {10.0f, 1.0f, 2.0f},
        .color = WORLD_COLOR_PLATFORM,
    };
    WORLD_CUBES(3) = (Geom){
        .translate = {12.5f, -1.0f, -5.5f},
        .scale = {10.0f, 1.0f, 1.0f},
        .color = WORLD_COLOR_PLATFORM,
    };
    WORLD_CUBES(4) = (Geom){
        .translate = {-5.0f, 0.5f, 0.0f},
        .scale = {0.5f, 3.5f, 15.0f},
        .color = WORLD_COLOR_WALL,
    };
    WORLD_CUBES(5) = (Geom){
        .translate = {10.0f, 0.5f, -5.0f},
        .scale = {20.0f, 3.5f, 0.5f},
        .color = WORLD_COLOR_WALL,
    };
    WORLD_CUBES(6) = (Geom){
        .translate = {2.5f, 0.5f, 5.0f},
        .scale = {7.5f, 3.5f, 0.5f},
        .color = WORLD_COLOR_WALL,
    };
    WORLD_CUBES(7) = (Geom){
        .translate = {2.5f, 0.5f, 5.0f},
        .scale = {0.5f, 3.5f, 7.5f},
        .color = WORLD_COLOR_WALL,
    };
    for (u32 i = 0; i < LEN_WORLD; ++i) {
        set_box_from_cube(&CUBES[OFFSET_WORLD + i], &BOXES[OFFSET_WORLD + i]);
    }
}

#endif
