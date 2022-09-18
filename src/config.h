#ifndef __CONFIG_H__
#define __CONFIG_H__

#include "geom.h"

#define PATH_CUBE_VERT "src/cube_vert.glsl"
#define PATH_CUBE_FRAG "src/cube_frag.glsl"

#define PATH_LINE_VERT "src/line_vert.glsl"
#define PATH_LINE_FRAG "src/line_frag.glsl"

#define PATH_SPRITE_VERT "src/sprite_vert.glsl"
#define PATH_SPRITE_FRAG "src/sprite_frag.glsl"

#define PATH_SPRITE_RUN "assets/sprite_run.png"

#define SPRITE_RUN_COLS 5
#define SPRITE_RUN_ROWS 8

#define WINDOW_WIDTH  1024
#define WINDOW_HEIGHT 768

#define BACKGROUND_COLOR 0.1f, 0.1f, 0.1f, 1.0f

#define FOV_DEGREES  45.0f
#define ASPECT_RATIO (((f32)WINDOW_WIDTH) / ((f32)WINDOW_HEIGHT))

#define VIEW_NEAR 0.01f
#define VIEW_FAR  100.0f
#define VIEW_UP   ((Vec3f){0.0f, 1.0f, 0.0f})

#define VIEW_FROM ((Vec3f){0.0f, 20.0f, 10.0f})
#define VIEW_TO   ((Vec3f){0.0f, 0.0f, 0.0f})

#define RUN      0.0016f
#define FRICTION 0.96f
#define DRAG     0.999f
#define GRAVITY  0.000675f

#define FLOOR -10.0f

#define CAMERA_LATENCY (1.0f / 235.0f)

#define FRAME_UPDATE_COUNT 8
#define FRAME_DURATION     (NANO_PER_SECOND / (60 + 1))
#define FRAME_UPDATE_STEP  (FRAME_DURATION / FRAME_UPDATE_COUNT)

#define PLAYER_TRANSLATE_INIT ((Vec3f){.x = 0.0f, .y = 5.0f, .z = 0.0f})

#define COLOR_PLAYER   ((Vec4f){0.8f, 0.85f, 0.95f, 0.5f})
#define COLOR_ENEMY    ((Vec4f){0.9f, 0.5f, 0.5f, 0.9f})
#define COLOR_PLATFORM ((Vec4f){0.2125f, 0.3f, 0.425f, 1.0f})
#define COLOR_WALL     ((Vec4f){0.1875f, 0.2f, 0.3f, 0.675f})

#define SCALE_PLAYER ((Vec3f){1.0f, 0.1f, 1.0f})
#define SCALE_ENEMY  ((Vec3f){1.0f, 1.0f, 1.0f})

static Cube CUBES[] = {
    {
        .translate = PLAYER_TRANSLATE_INIT,
        .scale = SCALE_PLAYER,
        .color = COLOR_PLAYER,
    },
    {
        .translate = {30.0f, 0.0f, 0.0f},
        .scale = SCALE_ENEMY,
        .color = COLOR_ENEMY,
    },
    {
        .translate = {25.0f, 0.0f, -5.0f},
        .scale = SCALE_ENEMY,
        .color = COLOR_ENEMY,
    },
    {
        .translate = {0.0f, -1.0f, 0.0f},
        .scale = {20.0f, 1.0f, 20.0f},
        .color = COLOR_PLATFORM,
    },
    {
        .translate = {25.0f, -1.0f, 0.0f},
        .scale = {20.0f, 1.0f, 20.0f},
        .color = COLOR_PLATFORM,
    },
    {
        .translate = {12.5f, -1.0f, 0.0f},
        .scale = {10.0f, 1.0f, 2.0f},
        .color = COLOR_PLATFORM,
    },
    {
        .translate = {12.5f, -1.0f, -5.5f},
        .scale = {10.0f, 1.0f, 1.0f},
        .color = COLOR_PLATFORM,
    },
    {
        .translate = {-5.0f, 0.5f, 0.0f},
        .scale = {0.5f, 3.5f, 15.0f},
        .color = COLOR_WALL,
    },
    {
        .translate = {10.0f, 0.5f, -5.0f},
        .scale = {20.0f, 3.5f, 0.5f},
        .color = COLOR_WALL,
    },
    {
        .translate = {2.5f, 0.5f, 5.0f},
        .scale = {7.5f, 3.5f, 0.5f},
        .color = COLOR_WALL,
    },
    {
        .translate = {2.5f, 0.5f, 5.0f},
        .scale = {0.5f, 3.5f, 7.5f},
        .color = COLOR_WALL,
    },
};

#define LEN_CUBES (sizeof(CUBES) / sizeof(CUBES[0]))

#define PLAYER (CUBES[0])

#endif
