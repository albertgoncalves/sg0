#ifndef __CONFIG_H__
#define __CONFIG_H__

#include "math.h"

#define WINDOW_WIDTH  1024
#define WINDOW_HEIGHT 768

#define BACKGROUND_COLOR 0.1f, 0.1f, 0.1f, 1.0f

#define FOV_DEGREES 45.0f

#define VIEW_NEAR 0.01f
#define VIEW_FAR  100.0f
#define VIEW_UP   ((Vec3f){0.0f, 1.0f, 0.0f})

#define VIEW_FROM ((Vec3f){0.0f, 20.0f, 10.0f})
#define VIEW_TO   ((Vec3f){0.0f, 0.0f, 0.0f})

#define RUN      0.0025f
#define FRICTION 0.9325f

#define CAMERA_LATENCY (1.0f / 225.0f)

#define PATH_DISPLAY_VERT "src/display_vert.glsl"
#define PATH_DISPLAY_FRAG "src/display_frag.glsl"

static Rect RECTS[] = {
    {
        {0},
        {1.0f, 1.0f, 1.0f},
        {0.8f, 0.85f, 0.95f, 0.9f},
    },
    {
        {0.0f, -1.0f, 0.0f},
        {20.f, 1.0f, 20.f},
        {0.2125f, 0.2125f, 0.2125f, 1.0f},
    },
    {
        {-5.0f, 0.5f, 0.0f},
        {0.5f, 3.5f, 15.0f},
        {0.1875f, 0.1875f, 0.1875f, 0.75f},
    },
    {
        {5.5f, 0.5f, -5.0f},
        {10.0f, 3.5f, 0.5f},
        {0.1875f, 0.1875f, 0.1875f, 0.75f},
    },
    {
        {2.5f, 0.5f, 5.0f},
        {7.5f, 3.5f, 0.5f},
        {0.1875f, 0.1875f, 0.1875f, 0.75f},
    },
};

#define LEN_RECTS (sizeof(RECTS) / sizeof(RECTS[0]))

#define PLAYER_POSITION (RECTS[0].translate)

#define FRAME_UPDATE_COUNT 8
#define FRAME_DURATION     (NANO_PER_SECOND / (60 + 1))
#define FRAME_UPDATE_STEP  (FRAME_DURATION / FRAME_UPDATE_COUNT)

#endif
