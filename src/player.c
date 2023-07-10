#include "player.h"

#include "sprite.h"
#include "world.h"

#define EPSILON 0.001f

#define NEAR_ZERO(x) ((-EPSILON < x) && (x < EPSILON))

#define RUN      0.00175f
#define FRICTION 0.96f
#define DRAG     0.999f

#define TRANSLATE ((Vec3f){0.0f, 2.0f, 0.0f})

#define SCALE_CUBE   ((Vec3f){1.0f, 0.1f, 1.0f})
#define SCALE_SPRITE ((Vec3f){2.0f, 2.0f, 1.0f})

#define COLOR_CUBE   ((Vec4f){0.75f, 0.75f, 0.75f, 1.0f})
#define COLOR_SPRITE ((Vec4f){1.0f, 1.0f, 1.0f, 1.0f})

#define SPRITE_TRANSLATE_Y 1.0f

#define PLAYER_BOX    BOXES[OFFSET_PLAYER]
#define PLAYER_SPRITE SPRITES[OFFSET_PLAYER]

#define SPRITE_COLS 5
#define SPRITE_ROWS 8

#define SPRITE_COLS_OFFSET 0
#define SPRITE_ROWS_OFFSET 0

#define SPRITE_TURN      (360.0f / SPRITE_ROWS)
#define SPRITE_TURN_HALF (SPRITE_TURN / 2.0f)

#define DIRECTION(polar_degrees) \
    (((u8)((polar_degrees + SPRITE_TURN_HALF) / SPRITE_TURN)) % SPRITE_ROWS)

static const u8 SPRITE_DIRECTIONS[SPRITE_ROWS] = {3, 4, 0, 7, 6, 5, 1, 2};

static Vec3f SPEED = {0};

extern Geom   CUBES[CAP_CUBES];
extern Sprite SPRITES[CAP_SPRITES];
extern Box    BOXES[CAP_CUBES];

extern u32 LEN_WORLD;

extern u64 SPRITE_TIME;

void player_init(void) {
    PLAYER_CUBE.translate = TRANSLATE;
    PLAYER_CUBE.scale = SCALE_CUBE;
    PLAYER_CUBE.color = COLOR_CUBE;

    PLAYER_SPRITE.geom.translate = TRANSLATE;
    PLAYER_SPRITE.geom.scale = SCALE_SPRITE;
    PLAYER_SPRITE.geom.color = COLOR_SPRITE;
}

void player_update(Vec3f move) {
    EXIT_IF(0.0f < SPEED.y);
    if (SPEED.y < 0.0f) {
        SPEED.x *= DRAG;
        SPEED.z *= DRAG;
    } else {
        SPEED.x += move.x * RUN;
        SPEED.z += move.z * RUN;
        SPEED.x *= FRICTION;
        SPEED.z *= FRICTION;
    }
    SPEED.y -= WORLD_GRAVITY;
    {
        Vec3f speed = SPEED;
        Vec3f remaining = SPEED;
        u8    hit = 0;
        for (u32 _ = 0; _ < 3; ++_) {
            PLAYER_BOX = geom_box(&PLAYER_CUBE);
            Collision collision = {0};
            for (u32 i = 0; i < LEN_WORLD; ++i) {
                const Collision candidate =
                    geom_collision(&PLAYER_BOX,
                                   &BOXES[OFFSET_WORLD + i],
                                   &speed);
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
                if (((*(const u32*)&candidate.time) ==
                     (*(const u32*)&collision.time)) &&
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
            SPEED.x = 0.0f;
        }
        if (hit & HIT_Y) {
            SPEED.y = 0.0f;
        }
        if (hit & HIT_Z) {
            SPEED.z = 0.0f;
        }
    }
    if (PLAYER_CUBE.translate.y < WORLD_FLOOR) {
        PLAYER_CUBE.translate = TRANSLATE;
        SPEED = (Vec3f){0};
    }
}

void player_animate(void) {
    PLAYER_SPRITE.geom.translate = PLAYER_CUBE.translate;
    PLAYER_SPRITE.geom.translate.y += SPRITE_TRANSLATE_Y;
    if (NEAR_ZERO(SPEED.x) && NEAR_ZERO(SPEED.z)) {
        PLAYER_SPRITE.col_row.x = 4;
        return;
    }
    const f32 polar_degrees = math_polar_degrees((Vec2f){
        .x = SPEED.x == 0.0f ? EPSILON : SPEED.x,
        .y = SPEED.z == 0.0f ? EPSILON : -SPEED.z,
    });
    PLAYER_SPRITE.col_row = (Vec2u){
        .x = SPRITE_COLS_OFFSET + ((SPRITE_TIME / 10000) % (SPRITE_COLS - 1)),
        .y = SPRITE_ROWS_OFFSET + SPRITE_DIRECTIONS[DIRECTION(polar_degrees)],
    };
}
