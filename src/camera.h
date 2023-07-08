#ifndef CAMERA_H
#define CAMERA_H

#include "math.h"
#include "window.h"

#define FOV_DEGREES  45.0f
#define ASPECT_RATIO (((f32)WINDOW_WIDTH) / ((f32)WINDOW_HEIGHT))

#define VIEW_NEAR 0.01f
#define VIEW_FAR  100.0f
#define VIEW_UP   ((Vec3f){0.0f, 1.0f, 0.0f})

#define VIEW_FROM ((Vec3f){0.0f, 22.5f, 10.0f})
#define VIEW_TO   ((Vec3f){0.0f, 0.0f, 0.0f})

static Vec3f VIEW_OFFSET = {0};

#define CAMERA_LATENCY 175.0f

static void update_camera(Vec3f target) {
    VIEW_OFFSET.x -= (VIEW_OFFSET.x - target.x) / CAMERA_LATENCY;
    VIEW_OFFSET.z -= (VIEW_OFFSET.z - target.z) / CAMERA_LATENCY;
}

static Mat4 get_view(void) {
    const Vec3f view_from = (Vec3f){
        .x = VIEW_FROM.x + VIEW_OFFSET.x,
        .y = VIEW_FROM.y + VIEW_OFFSET.y,
        .z = VIEW_FROM.z + VIEW_OFFSET.z,
    };
    const Vec3f view_to = (Vec3f){
        .x = VIEW_TO.x + VIEW_OFFSET.x,
        .y = VIEW_TO.y + VIEW_OFFSET.y,
        .z = VIEW_TO.z + VIEW_OFFSET.z,
    };
    return look_at(view_from, view_to, VIEW_UP);
}

#endif
