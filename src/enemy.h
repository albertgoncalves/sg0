#ifndef ENEMY_H
#define ENEMY_H

#include "memory.h"
#include "sprite.h"

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

static Enemy ENEMIES[CAP_ENEMIES];

static void init_enemies(void) {
    LEN_ENEMIES = 4;
    EXIT_IF(CAP_ENEMIES < LEN_ENEMIES);
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

#define ENEMY_SPRITE_TURN      (360.0f / SPRITE_ENEMY_ROWS)
#define ENEMY_SPRITE_TURN_HALF (ENEMY_SPRITE_TURN / 2.0f)
        ENEMY_SPRITES(i).col_row = (Vec2u){
            .x = SPRITE_ENEMY_COLS_OFFSET,
            .y = SPRITE_ENEMY_ROWS_OFFSET +
                 SPRITE_DIRECTIONS_ENEMY[((u8)((ENEMIES[i].polar_degrees +
                                                ENEMY_SPRITE_TURN_HALF) /
                                               ENEMY_SPRITE_TURN)) %
                                         SPRITE_ENEMY_ROWS],
        };
#undef ENEMY_SPRITE_TURN_HALF
#undef ENEMY_SPRITE_TURN

        const f32  polar_radians = get_radians(ENEMIES[i].polar_degrees);
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
        LINES[i] = get_geom_between(&ENEMY_CUBES(i), &target);
        LINES[i].translate.y += ENEMY_LINE_TRANSLATE_Y;
        LINES[i].color.w = ENEMY_LINE_COLOR_W;
    }
}

#endif
