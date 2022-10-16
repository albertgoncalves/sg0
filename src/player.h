#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "memory.h"
#include "world.h"

#define PLAYER_RUN      0.00175f
#define PLAYER_FRICTION 0.96f
#define PLAYER_DRAG     0.999f

#define PLAYER_TRANSLATE ((Vec3f){0.0f, 2.0f, 0.0f})

#define PLAYER_SCALE_CUBE   ((Vec3f){1.0f, 0.1f, 1.0f})
#define PLAYER_SCALE_SPRITE ((Vec3f){2.0f, 2.0f, 1.0f})

#define PLAYER_COLOR_CUBE   ((Vec4f){0.75f, 0.75f, 0.75f, 1.0f})
#define PLAYER_COLOR_SPRITE ((Vec4f){1.0f, 1.0f, 1.0f, 1.0f})

#define PLAYER_SPRITE_TRANSLATE_Y 1.0f

static Vec3f PLAYER_SPEED = {0};

#define PLAYER_CUBE   CUBES[OFFSET_PLAYER]
#define PLAYER_SPRITE SPRITES[OFFSET_PLAYER]

static void init_player(void) {
    PLAYER_CUBE.translate = PLAYER_TRANSLATE;
    PLAYER_CUBE.scale = PLAYER_SCALE_CUBE;
    PLAYER_CUBE.color = PLAYER_COLOR_CUBE;

    PLAYER_SPRITE.geom.translate = PLAYER_TRANSLATE;
    PLAYER_SPRITE.geom.scale = PLAYER_SCALE_SPRITE;
    PLAYER_SPRITE.geom.color = PLAYER_COLOR_SPRITE;
}

static void update_player(Vec3f move) {
    EXIT_IF(0.0f < PLAYER_SPEED.y);
    if (PLAYER_SPEED.y < 0.0f) {
        PLAYER_SPEED.x *= PLAYER_DRAG;
        PLAYER_SPEED.z *= PLAYER_DRAG;
    } else {
        PLAYER_SPEED.x += move.x * PLAYER_RUN;
        PLAYER_SPEED.z += move.z * PLAYER_RUN;
        PLAYER_SPEED.x *= PLAYER_FRICTION;
        PLAYER_SPEED.z *= PLAYER_FRICTION;
    }
    PLAYER_SPEED.y -= WORLD_GRAVITY;
    {
        Vec3f speed = PLAYER_SPEED;
        Vec3f remaining = PLAYER_SPEED;
        u8    hit = 0;
        for (u32 _ = 0; _ < 3; ++_) {
            set_box_from_cube(&PLAYER_CUBE, &BOXES[OFFSET_PLAYER]);
            Collision collision = {0};
            for (u32 i = 0; i < LEN_WORLD; ++i) {
                const Collision candidate =
                    get_box_collision(&BOXES[OFFSET_PLAYER],
                                      &BOXES[OFFSET_WORLD + i],
                                      &speed);
                if (!candidate.hit) {
                    continue;
                }
                if (!collision.hit || (candidate.time < collision.time) ||
                    ((candidate.time == collision.time) &&
                     (collision.overlap < candidate.overlap)))
                {
                    collision = candidate;
                }
            }
            if (!collision.hit) {
                PLAYER_CUBE.translate.x += speed.x;
                PLAYER_CUBE.translate.y += speed.y;
                PLAYER_CUBE.translate.z += speed.z;
                break;
            }
            speed.x *= collision.time;
            speed.y *= collision.time;
            speed.z *= collision.time;
            PLAYER_CUBE.translate.x += speed.x;
            PLAYER_CUBE.translate.y += speed.y;
            PLAYER_CUBE.translate.z += speed.z;
            remaining.x -= speed.x;
            remaining.y -= speed.y;
            remaining.z -= speed.z;
            switch (collision.hit) {
            case HIT_X: {
                remaining.x = 0.0f;
                break;
            }
            case HIT_Y: {
                remaining.y = 0.0f;
                break;
            }
            case HIT_Z: {
                remaining.z = 0.0f;
                break;
            }
            case HIT_NONE:
            default: {
                EXIT();
            }
            }
            speed.x = remaining.x;
            speed.y = remaining.y;
            speed.z = remaining.z;
            hit |= collision.hit;
        }
        if (hit & HIT_X) {
            PLAYER_SPEED.x = 0.0f;
        }
        if (hit & HIT_Y) {
            PLAYER_SPEED.y = 0.0f;
        }
        if (hit & HIT_Z) {
            PLAYER_SPEED.z = 0.0f;
        }
    }
    if (PLAYER_CUBE.translate.y < WORLD_FLOOR) {
        PLAYER_CUBE.translate = PLAYER_TRANSLATE;
        PLAYER_SPEED = (Vec3f){0};
    }
}

static void animate_player(void) {
    PLAYER_SPRITE.geom.translate = PLAYER_CUBE.translate;
    PLAYER_SPRITE.geom.translate.y += PLAYER_SPRITE_TRANSLATE_Y;
    const Vec2f speed = {
        .x = PLAYER_SPEED.x,
        .y = -PLAYER_SPEED.z,
    };
    if (NEAR_ZERO(speed.x) && NEAR_ZERO(speed.y)) {
        PLAYER_SPRITE.col_row.x = 4;
        return;
    }
    const f32 angle = get_polar_degrees((Vec2f){
        .x = speed.x == 0.0f ? EPSILON : speed.x,
        .y = speed.y == 0.0f ? EPSILON : speed.y,
    });
#define PLAYER_SPRITE_TURN      (360.0f / SPRITE_PLAYER_ROWS)
#define PLAYER_SPRITE_TURN_HALF (PLAYER_SPRITE_TURN / 2.0f)
    PLAYER_SPRITE.col_row = (Vec2u){
        .x = SPRITE_PLAYER_COLS_OFFSET + ((SPRITE_TIME / 10000) % 4),
        .y = SPRITE_PLAYER_ROWS_OFFSET +
             SPRITE_DIRECTIONS_PLAYER[((u8)((angle + PLAYER_SPRITE_TURN_HALF) /
                                            PLAYER_SPRITE_TURN)) %
                                      SPRITE_PLAYER_ROWS],
    };
#undef PLAYER_SPRITE_TURN_HALF
#undef PLAYER_SPRITE_TURN
}

#endif
