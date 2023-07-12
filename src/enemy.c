#include "enemy.h"

#include "sprite.h"

#include <math.h>

typedef struct {
    Vec3f translate;
    Vec3f speed;
    f32   polar_degrees;
} Enemy;

#define RUN      0.001f
#define FRICTION 0.975f

#define CUBE_TRANSLATE_Y   -0.45f
#define SPRITE_TRANSLATE_Y 0.69f

#define SCALE_CUBE   ((Vec3f){1.0f, 0.1f, 1.0f})
#define SCALE_SPRITE ((Vec3f){2.0f, 3.0f, 1.0f})

#define COLOR_CUBE   ((Vec4f){0.65f, 0.325f, 0.325f, 1.0f})
#define COLOR_SPRITE ((Vec4f){0.875f, 0.25f, 0.25f, 1.0f})

#define LINE_RADIUS      7.5f
#define LINE_TRANSLATE_Y 1.75f
#define LINE_COLOR_ALPHA 0.75f

#define ENEMY_CUBES(i)   CUBES[OFFSET_ENEMIES + i]
#define ENEMY_SPRITES(i) SPRITES[OFFSET_ENEMIES + i]

#define SPRITE_ROWS 4

#define SPRITE_COLS_OFFSET 5
#define SPRITE_ROWS_OFFSET 0

#define SPRITE_TURN      (360.0f / SPRITE_ROWS)
#define SPRITE_TURN_HALF (SPRITE_TURN / 2.0f)

#define DIRECTION(polar_degrees) \
    (((u8)((polar_degrees + SPRITE_TURN_HALF) / SPRITE_TURN)) % SPRITE_ROWS)

static const u8 SPRITE_DIRECTIONS[SPRITE_ROWS] = {2, 1, 3, 0};

static Enemy ENEMIES[CAP_ENEMIES];
static u32   LEN_ENEMIES = 0;

static void rotate(const Enemy* enemy,
                   const Geom*  cube,
                   Sprite*      sprite,
                   Geom*        line) {
    sprite->col_row = (Vec2u){
        .x = SPRITE_COLS_OFFSET,
        .y = SPRITE_ROWS_OFFSET +
             SPRITE_DIRECTIONS[DIRECTION(enemy->polar_degrees)],
    };

    const f32  polar_radians = math_radians(enemy->polar_degrees);
    const Geom target = {
        .translate =
            (Vec3f){
                .x = enemy->translate.x + (LINE_RADIUS * cosf(polar_radians)),
                .y = enemy->translate.y,
                .z = enemy->translate.z - (LINE_RADIUS * sinf(polar_radians)),
            },
        .scale = SCALE_SPRITE,
        .color = COLOR_SPRITE,
    };
    *line = geom_between(cube, &target);
    line->translate.y += LINE_TRANSLATE_Y;
    line->color.w = LINE_COLOR_ALPHA;
}

void enemy_init(void) {
    LEN_ENEMIES = 4;
    EXIT_IF(CAP_ENEMIES < LEN_ENEMIES);

    LEN_SPRITES = CAP_PLAYER + LEN_ENEMIES;
    EXIT_IF(CAP_SPRITES < LEN_SPRITES);

    LEN_LINES = LEN_ENEMIES;
    EXIT_IF(CAP_LINES < LEN_LINES);

    ENEMIES[0] = (Enemy){
        .translate = (Vec3f){30.0f, CUBE_TRANSLATE_Y, 0.0f},
        .polar_degrees = 0.0f,
    };
    ENEMIES[1] = (Enemy){
        .translate = (Vec3f){25.0f, CUBE_TRANSLATE_Y, -5.0f},
        .polar_degrees = 90.0f,
    };
    ENEMIES[2] = (Enemy){
        .translate = (Vec3f){20.0f, CUBE_TRANSLATE_Y, 0.0f},
        .polar_degrees = 180.0f,
    };
    ENEMIES[3] = (Enemy){
        .translate = (Vec3f){25.0f, CUBE_TRANSLATE_Y, 5.0f},
        .polar_degrees = 270.0f,
    };
    for (u32 i = 0; i < LEN_ENEMIES; ++i) {
        ENEMY_CUBES(i) = (Geom){
            .translate = ENEMIES[i].translate,
            .scale = SCALE_CUBE,
            .color = COLOR_CUBE,
        };
        ENEMY_SPRITES(i).geom.translate = ENEMIES[i].translate;
        ENEMY_SPRITES(i).geom.translate.y += SPRITE_TRANSLATE_Y;
        ENEMY_SPRITES(i).geom.scale = SCALE_SPRITE;
        ENEMY_SPRITES(i).geom.color = COLOR_SPRITE;
        rotate(&ENEMIES[i], &ENEMY_CUBES(i), &ENEMY_SPRITES(i), &LINES[i]);
    }
}

void enemy_update(void) {
    for (u32 i = 0; i < LEN_ENEMIES; ++i) {
        ENEMIES[i].polar_degrees += 0.1f;
        while (360.0f <= ENEMIES[i].polar_degrees) {
            ENEMIES[i].polar_degrees -= 360.0f;
        }
        {
            const Vec3f move = (Vec3f){.x = -0.05f};
            ENEMIES[i].speed.x += move.x * RUN;
            ENEMIES[i].speed.z += move.z * RUN;
            ENEMIES[i].speed.x *= FRICTION;
            ENEMIES[i].speed.z *= FRICTION;
            ENEMIES[i].translate.x += ENEMIES[i].speed.x;
            ENEMIES[i].translate.z += ENEMIES[i].speed.z;
        }

        rotate(&ENEMIES[i], &ENEMY_CUBES(i), &ENEMY_SPRITES(i), &LINES[i]);
        ENEMY_CUBES(i).translate.x = ENEMIES[i].translate.x;
        ENEMY_CUBES(i).translate.z = ENEMIES[i].translate.z;
        ENEMY_SPRITES(i).geom.translate.x = ENEMIES[i].translate.x;
        ENEMY_SPRITES(i).geom.translate.z = ENEMIES[i].translate.z;
    }
}
