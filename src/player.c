#include "player.h"

#include "graphics.h"
#include "time.h"
#include "world.h"

#include <math.h>

#define RUN      0.002125f
#define FRICTION 0.9425f
#define DRAG     0.999f

#define HALT 0.000375f

#define TRANSLATE ((Vec3f){-3.0f, 0.5f, 13.75f})

#define SCALE_CUBE   ((Vec3f){1.0f, 0.1f, 1.0f})
#define SCALE_SPRITE ((Vec3f){2.0f, 2.0f, 1.0f})

#define COLOR_CUBE   ((Vec4f){1.0f, 1.0f, 1.0f, 0.15f})
#define COLOR_SPRITE ((Vec4f){1.0f, 1.0f, 1.0f, 1.0f})

#define TRANSLATE_Y_SPRITE 1.0f

#define PLAYER_SPRITE SPRITES[OFFSET_PLAYER - LEN_WORLD]

#define SPRITE_COLS 5
#define SPRITE_ROWS 8

#define OFFSET_SPRITE_COLS 0
#define OFFSET_SPRITE_ROWS 0

#define SPRITE_TURN (360.0f / SPRITE_ROWS)

#define SPRITE_RATE (0.125 * NANOS_PER_SECOND)

#define DIRECTION(polar_degrees) \
    (((u8)((polar_degrees + (SPRITE_TURN / 2.0f)) / SPRITE_TURN)) % SPRITE_ROWS)

static const u8 SPRITE_DIRECTIONS[SPRITE_ROWS] = {3, 4, 0, 7, 6, 5, 1, 2};

void player_init(void) {
    PLAYER_SPEED.x = 0;
    PLAYER_SPEED.y = 0;
    PLAYER_SPEED.z = 0;

    PLAYER_CUBE.translate = TRANSLATE;
    PLAYER_CUBE.scale = SCALE_CUBE;
    PLAYER_CUBE.color = COLOR_CUBE;

    PLAYER_SPRITE.geom.translate = TRANSLATE;
    PLAYER_SPRITE.geom.scale = SCALE_SPRITE;
    PLAYER_SPRITE.geom.color = COLOR_SPRITE;

    PLAYER_SPRITE.col_row.x = 0;
    PLAYER_SPRITE.col_row.y = 0;
}

void player_update(Vec3f move, f32 t) {
    EXIT_IF(0.0f < PLAYER_SPEED.y);

    const Vec3f prev = PLAYER_SPEED;

    if (PLAYER_SPEED.y < 0.0f) {
        PLAYER_SPEED.x *= DRAG;
        PLAYER_SPEED.z *= DRAG;
    } else {
        PLAYER_SPEED.x += move.x * RUN;
        PLAYER_SPEED.z += move.z * RUN;
        PLAYER_SPEED.x *= FRICTION;
        PLAYER_SPEED.z *= FRICTION;
    }
    PLAYER_SPEED.y -= WORLD_GRAVITY;

    PLAYER_SPEED = math_lerp_vec3f(prev, PLAYER_SPEED, t);

    {
        Vec3f speed = PLAYER_SPEED;
        Vec3f remaining = PLAYER_SPEED;
        u8    hit = 0;
        for (u32 _ = 0; _ < 3; ++_) {
            const Box player_box = geom_box(&PLAYER_CUBE);
            Collision collision = {0};
            for (u32 i = 0; i < LEN_WORLD; ++i) {
                // NOTE: This *should* work because `BOXES` is sorted by `x`.
                if (BOXES[i].right_top_front.x < (player_box.left_bottom_back.x + PLAYER_SPEED.x)) {
                    continue;
                }
                // NOTE: See `https://leanrada.com/notes/sweep-and-prune/`.
                if ((player_box.right_top_front.x + PLAYER_SPEED.x) < BOXES[i].left_bottom_back.x) {
                    break;
                }

                const Collision candidate = geom_collision(&player_box, &BOXES[i], &speed);
                if (!candidate.hit) {
                    continue;
                }
                if (!collision.hit) {
                    collision = candidate;
                    continue;
                }
                if (candidate.time < collision.time) {
                    collision = candidate;
                    continue;
                }
                if (((*(const u32*)&candidate.time) == (*(const u32*)&collision.time)) &&
                    (collision.overlap < candidate.overlap))
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
        PLAYER_CUBE.translate = TRANSLATE;
        PLAYER_SPEED = (Vec3f){0};
    }
}

void player_animate(void) {
    PLAYER_SPRITE.geom.translate = PLAYER_CUBE.translate;
    PLAYER_SPRITE.geom.translate.y += TRANSLATE_Y_SPRITE;
    if ((fabsf(PLAYER_SPEED.x) <= HALT) && (fabsf(PLAYER_SPEED.z) <= HALT)) {
        PLAYER_SPRITE.col_row.x = 4;
        return;
    }
    const f32 polar_degrees = math_polar_degrees((Vec2f){
        .x = PLAYER_SPEED.x,
        .y = -PLAYER_SPEED.z,
    });
    PLAYER_SPRITE.col_row = (Vec2u){
        .x = OFFSET_SPRITE_COLS + ((time_now() / ((u64)SPRITE_RATE)) % (SPRITE_COLS - 1)),
        .y = OFFSET_SPRITE_ROWS + SPRITE_DIRECTIONS[DIRECTION(polar_degrees)],
    };
}
