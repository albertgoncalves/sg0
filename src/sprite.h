#ifndef __SPRITE_H__
#define __SPRITE_H__

#include "math.h"

#define PATH_SPRITE_RUN "assets/sprite_run.png"

#define SPRITE_RUN_COLS 5
#define SPRITE_RUN_ROWS 8

static u64 SPRITE_TIME = 0;

#define SPRITE_UPDATE_STEP 250

typedef struct {
    Vec3f translate;
    Vec3f scale;
    Vec4f color;
    Vec2u cell;
} Sprite;

static void animate_sprite_run(Vec2f speed, Vec2u* cell) {
    if (NEAR_ZERO(speed.x) && NEAR_ZERO(speed.y)) {
        cell->x = 4;
        return;
    }
    const f32 angle = polar_degrees((Vec2f){
        .x = speed.x == 0.0f ? EPSILON : speed.x,
        .y = speed.y == 0.0f ? EPSILON : speed.y,
    });
    const u8  directions[SPRITE_RUN_ROWS] = {3, 4, 0, 7, 6, 5, 1, 2};
    cell->y = directions[((u8)((angle + 22.5f) / 45.0f)) % SPRITE_RUN_ROWS];
    cell->x = (SPRITE_TIME / 10000) % 4;
}

#endif
