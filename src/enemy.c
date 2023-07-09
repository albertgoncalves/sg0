#include "enemy.h"

#include "sprite.h"

#include <math.h>

typedef struct {
    Vec3f translate;
    f32   polar_degrees;
} Enemy;

#define ENEMY_CUBE_TRANSLATE_Y   -0.45f
#define ENEMY_SPRITE_TRANSLATE_Y 0.69f

#define ENEMY_SCALE_CUBE   ((Vec3f){1.0f, 0.1f, 1.0f})
#define ENEMY_SCALE_SPRITE ((Vec3f){2.0f, 3.0f, 1.0f})

#define ENEMY_COLOR_CUBE   ((Vec4f){0.65f, 0.325f, 0.325f, 1.0f})
#define ENEMY_COLOR_SPRITE ((Vec4f){0.875f, 0.25f, 0.25f, 1.0f})

#define ENEMY_LINE_RADIUS      3.5f
#define ENEMY_LINE_TRANSLATE_Y 1.75f
#define ENEMY_LINE_COLOR_W     0.75f

#define ENEMY_CUBES(i)   CUBES[OFFSET_ENEMIES + i]
#define ENEMY_SPRITES(i) SPRITES[OFFSET_ENEMIES + i]

// #define SPRITE_ENEMY_COLS 4
#define SPRITE_ENEMY_ROWS 4

#define SPRITE_ENEMY_COLS_OFFSET 5
#define SPRITE_ENEMY_ROWS_OFFSET 0

#define ENEMY_SPRITE_TURN      (360.0f / SPRITE_ENEMY_ROWS)
#define ENEMY_SPRITE_TURN_HALF (ENEMY_SPRITE_TURN / 2.0f)

static const u8 SPRITE_DIRECTIONS_ENEMY[SPRITE_ENEMY_ROWS] = {2, 1, 3, 0};

static Enemy ENEMIES[CAP_ENEMIES];
static u32   LEN_ENEMIES = 0;

extern Geom CUBES[CAP_CUBES];

extern Sprite SPRITES[CAP_SPRITES];
extern u32    LEN_SPRITES;

extern Geom LINES[CAP_LINES];
extern u32  LEN_LINES;

void enemy_init(void) {
    LEN_ENEMIES = 4;
    EXIT_IF(CAP_ENEMIES < LEN_ENEMIES);

    LEN_SPRITES = CAP_PLAYER + LEN_ENEMIES;
    EXIT_IF(CAP_SPRITES < LEN_SPRITES);

    LEN_LINES = LEN_ENEMIES;
    EXIT_IF(CAP_LINES < LEN_LINES);

    ENEMIES[0] = (Enemy){
        .translate = (Vec3f){30.0f, ENEMY_CUBE_TRANSLATE_Y, 0.0f},
        .polar_degrees = 0.0f,
    };
    ENEMIES[1] = (Enemy){
        .translate = (Vec3f){25.0f, ENEMY_CUBE_TRANSLATE_Y, -5.0f},
        .polar_degrees = 90.0f,
    };
    ENEMIES[2] = (Enemy){
        .translate = (Vec3f){20.0f, ENEMY_CUBE_TRANSLATE_Y, 0.0f},
        .polar_degrees = 180.0f,
    };
    ENEMIES[3] = (Enemy){
        .translate = (Vec3f){25.0f, ENEMY_CUBE_TRANSLATE_Y, 5.0f},
        .polar_degrees = 270.0f,
    };
    for (u32 i = 0; i < LEN_ENEMIES; ++i) {
        ENEMY_CUBES(i) = (Geom){
            .translate = ENEMIES[i].translate,
            .scale = ENEMY_SCALE_CUBE,
            .color = ENEMY_COLOR_CUBE,
        };
        ENEMY_SPRITES(i).geom.translate = ENEMIES[i].translate;
        ENEMY_SPRITES(i).geom.translate.y += ENEMY_SPRITE_TRANSLATE_Y;
        ENEMY_SPRITES(i).geom.scale = ENEMY_SCALE_SPRITE;
        ENEMY_SPRITES(i).geom.color = ENEMY_COLOR_SPRITE;

        ENEMY_SPRITES(i).col_row = (Vec2u){
            .x = SPRITE_ENEMY_COLS_OFFSET,
            .y = SPRITE_ENEMY_ROWS_OFFSET +
                 SPRITE_DIRECTIONS_ENEMY[((u8)((ENEMIES[i].polar_degrees +
                                                ENEMY_SPRITE_TURN_HALF) /
                                               ENEMY_SPRITE_TURN)) %
                                         SPRITE_ENEMY_ROWS],
        };

        const f32  polar_radians = math_radians(ENEMIES[i].polar_degrees);
        const Geom target = {
            .translate =
                (Vec3f){
                    .x = ENEMIES[i].translate.x +
                         (ENEMY_LINE_RADIUS * cosf(polar_radians)),
                    .y = ENEMIES[i].translate.y,
                    .z = ENEMIES[i].translate.z -
                         (ENEMY_LINE_RADIUS * sinf(polar_radians)),
                },
            .scale = ENEMY_SCALE_SPRITE,
            .color = ENEMY_COLOR_SPRITE,
        };
        LINES[i] = geom_between(&ENEMY_CUBES(i), &target);
        LINES[i].translate.y += ENEMY_LINE_TRANSLATE_Y;
        LINES[i].color.w = ENEMY_LINE_COLOR_W;
    }
}
