#include "enemy.h"

#include "graphics.h"
#include "pcg_rng.h"
#include "player.h"
#include "time.h"
#include "world.h"

#include <math.h>

#define CAP_WAYPOINTS (1 << 5)

static Waypoint WAYPOINTS[CAP_WAYPOINTS];

#define RUN      0.0002f
#define FRICTION 0.975f

#define TURN 2.25f

#define HALT      0.0001f
#define THRESHOLD 0.025f

#define FOV_DEGREES 70.0f

#define TRANSLATE_Y_CUBE   -0.45f
#define TRANSLATE_Y_LINE   1.975f
#define TRANSLATE_Y_SPRITE 0.485f

#define SCALE_CUBE   ((Vec3f){1.0f, 0.1f, 1.0f})
#define SCALE_SPRITE ((Vec3f){2.0f, 2.85f, 1.0f})

#define COLOR_CUBE   ((Vec4f){0.875f, 0.25f, 0.25f, 0.3f})
#define COLOR_SPRITE ((Vec4f){0.875f, 0.25f, 0.25f, 1.0f})
#define COLOR_LINE   ((Vec4f){0.875f, 0.25f, 0.25f, 0.185f})

#define RADIUS_LINE 4.0f

#define ENEMY_CUBES(i)   CUBES[OFFSET_ENEMIES + i]
#define ENEMY_SPRITES(i) SPRITES[(OFFSET_ENEMIES + i) - LEN_WORLD]

#define SPRITE_ROWS 4

#define OFFSET_SPRITE_COLS 5
#define OFFSET_SPRITE_ROWS 0

#define SPRITE_TURN (360.0f / SPRITE_ROWS)

#define SPRITE_RATE 250000000

#define DIRECTION(polar_degrees)                                    \
    (((u8)((polar_degrees + (SPRITE_TURN / 2.0f)) / SPRITE_TURN)) % \
     SPRITE_ROWS)

static const u8 SPRITE_COL_INDICES[] = {1, 2, 3, 2};
#define SPRITE_COLS \
    (sizeof(SPRITE_COL_INDICES) / sizeof(SPRITE_COL_INDICES[0]))

static const u8 SPRITE_DIRECTIONS[SPRITE_ROWS] = {2, 1, 3, 0};

static const Vec2f POSITIONS[] = {
    {-13.5f, 13.5f},  // 0
    {-13.5f, -23.5f}, // 1
    {14.0f, -23.5f},  // 2
    {14.0f, -11.0f},  // 3
    {24.5f, -11.0f},  // 4
    {24.5f, 11.0f},   // 5
    {13.5f, 11.0f},   // 6
    {13.5f, 13.5f},   // 7

    {0.6f, 13.5f},   // 8
    {0.6f, -4.0f},   // 9
    {-13.5f, -4.0f}, // 10
    {0.6f, -23.5f},  // 11
    {8.0f, -23.5f},  // 12
    {8.0f, -6.5f},   // 13
    {0.6f, -6.5f},   // 14

    {19.0f, -11.0f}, // 15
    {19.0f, 11.0f},  // 16

    {13.5f, 10.0f}, // 17
    {13.5f, -6.5f}, // 18
    {4.35f, -6.5f}, // 19
};

Enemy enemy_lerp(Enemy l, Enemy r, f32 t) {
    return (Enemy){
        .translate = math_lerp_vec2f(l.translate, r.translate, t),
        .speed = math_lerp_vec2f(l.speed, r.speed, t),
        .polar_degrees = math_lerp_f32(l.polar_degrees, r.polar_degrees, t),
        .waypoint = (const Waypoint*)math_lerp_pointer((const void*)l.waypoint,
                                                       (const void*)r.waypoint,
                                                       t),
        .player_in_view =
            math_lerp_bool(l.player_in_view, r.player_in_view, t),
    };
}

void enemy_init(void) {
    PLAYER_IN_VIEW = FALSE;

    LEN_ENEMIES = 4;
    EXIT_IF(CAP_ENEMIES < LEN_ENEMIES);

    u32 len_waypoints = 0;
    {
        const u8  indices[] = {0, 1, 2, 3, 4, 5, 6, 7};
        const u32 offset = len_waypoints;
#define N (sizeof(indices) / sizeof(indices[0]))
        for (u32 i = 0; i < N; ++i) {
            EXIT_IF(CAP_WAYPOINTS < len_waypoints);
            WAYPOINTS[len_waypoints++] = (Waypoint){
                .translate = POSITIONS[indices[i]],
                .next = &WAYPOINTS[((i + 1) % N) + offset],
            };
        }
        const Waypoint* waypoint =
            &WAYPOINTS[pcg_rng_random_bounded_u32(N) + offset];
        ENEMIES[0].translate = waypoint->translate;
        ENEMIES[0].waypoint = waypoint;
#undef N
    }
    {
        const u8  indices[] = {8, 9, 10, 9, 11, 12, 13, 14};
        const u32 offset = len_waypoints;
#define N (sizeof(indices) / sizeof(indices[0]))
        for (u32 i = 0; i < N; ++i) {
            EXIT_IF(CAP_WAYPOINTS < len_waypoints);
            WAYPOINTS[len_waypoints++] = (Waypoint){
                .translate = POSITIONS[indices[i]],
                .next = &WAYPOINTS[((i + 1) % N) + offset],
            };
        }
        const Waypoint* waypoint =
            &WAYPOINTS[pcg_rng_random_bounded_u32(N) + offset];
        ENEMIES[1].translate = waypoint->translate;
        ENEMIES[1].waypoint = waypoint;
#undef N
    }
    {
        const u8  indices[] = {15, 16};
        const u32 offset = len_waypoints;
#define N (sizeof(indices) / sizeof(indices[0]))
        for (u32 i = 0; i < N; ++i) {
            EXIT_IF(CAP_WAYPOINTS < len_waypoints);
            WAYPOINTS[len_waypoints++] = (Waypoint){
                .translate = POSITIONS[indices[i]],
                .next = &WAYPOINTS[((i + 1) % N) + offset],
            };
        }
        const Waypoint* waypoint =
            &WAYPOINTS[pcg_rng_random_bounded_u32(N) + offset];
        ENEMIES[2].translate = waypoint->translate;
        ENEMIES[2].waypoint = waypoint;
#undef N
    }
    {
        const u8  indices[] = {17, 18, 19, 18};
        const u32 offset = len_waypoints;
#define N (sizeof(indices) / sizeof(indices[0]))
        for (u32 i = 0; i < N; ++i) {
            EXIT_IF(CAP_WAYPOINTS < len_waypoints);
            WAYPOINTS[len_waypoints++] = (Waypoint){
                .translate = POSITIONS[indices[i]],
                .next = &WAYPOINTS[((i + 1) % N) + offset],
            };
        }
        const Waypoint* waypoint =
            &WAYPOINTS[pcg_rng_random_bounded_u32(N) + offset];
        ENEMIES[3].translate = waypoint->translate;
        ENEMIES[3].waypoint = waypoint;
#undef N
    }

    for (u32 i = 0; i < LEN_ENEMIES; ++i) {
        ENEMIES[i].speed.x = 0.0f;
        ENEMIES[i].speed.y = 0.0f;

        {
            const Waypoint* waypoint = ENEMIES[i].waypoint->next;
            ENEMIES[i].polar_degrees = math_polar_degrees((Vec2f){
                .x = waypoint->translate.x - ENEMIES[i].translate.x,
                .y = -(waypoint->translate.y - ENEMIES[i].translate.y),
            });
        }

        ENEMY_CUBES(i) = (Geom){
            .translate = {.y = TRANSLATE_Y_CUBE},
            .scale = SCALE_CUBE,
            .color = COLOR_CUBE,
        };

        ENEMY_SPRITES(i).geom.translate.y = TRANSLATE_Y_SPRITE;
        ENEMY_SPRITES(i).geom.scale = SCALE_SPRITE;
        ENEMY_SPRITES(i).geom.color = COLOR_SPRITE;

        ENEMY_SPRITES(i).col_row.x = 0;
        ENEMY_SPRITES(i).col_row.y = 0;
    }
}

static Bool intersects(const Box* box, Vec2f line[2]) {
    const f32 left = box->left_bottom_back.x;
    const f32 back = box->left_bottom_back.z;
    const f32 right = box->right_top_front.x;
    const f32 front = box->right_top_front.z;

    const Vec2f points[] = {
        {left, back},
        {right, back},
        {right, front},
        {left, front},
    };
    return geom_intersects((Vec2f[2]){points[0], points[1]}, line) ||
           geom_intersects((Vec2f[2]){points[1], points[2]}, line) ||
           geom_intersects((Vec2f[2]){points[2], points[3]}, line) ||
           geom_intersects((Vec2f[2]){points[3], points[0]}, line);
}

void enemy_update(void) {
    PLAYER_IN_VIEW = FALSE;
    for (u32 i = 0; i < LEN_ENEMIES; ++i) {
        const Waypoint* waypoint = ENEMIES[i].waypoint;

        Vec3f distance = (Vec3f){
            .x = waypoint->translate.x - ENEMIES[i].translate.x,
            .y = 0.0f,
            .z = waypoint->translate.y - ENEMIES[i].translate.y,
        };
        if ((fabsf(distance.x - ENEMIES[i].speed.x) < THRESHOLD) &&
            (fabsf(distance.z - ENEMIES[i].speed.y) < THRESHOLD))
        {
            ENEMIES[i].speed.x = 0.0f;
            ENEMIES[i].speed.y = 0.0f;
            ENEMIES[i].translate.x = waypoint->translate.x;
            ENEMIES[i].translate.y = waypoint->translate.y;

            ENEMIES[i].waypoint = ENEMIES[i].waypoint->next;
            waypoint = ENEMIES[i].waypoint;
            distance = (Vec3f){
                .x = waypoint->translate.x - ENEMIES[i].translate.x,
                .y = 0.0f,
                .z = waypoint->translate.y - ENEMIES[i].translate.y,
            };
        }

        {
            const Vec3f move = math_normalize(distance);

            const f32 polar_degrees_move = math_polar_degrees((Vec2f){
                .x = move.x,
                .y = -move.z,
            });

            f32 angle = polar_degrees_move - ENEMIES[i].polar_degrees;
            {
                const f32 reverse = angle + 360.0f;
                angle = fabsf(reverse) < fabsf(angle) ? reverse : angle;
            }
            {
                const f32 reverse = angle - 360.0f;
                angle = fabsf(reverse) < fabsf(angle) ? reverse : angle;
            }

            if (fabsf(angle) < TURN) {
                ENEMIES[i].speed.x += move.x * RUN;
                ENEMIES[i].speed.y += move.z * RUN;
                ENEMIES[i].speed.x *= FRICTION;
                ENEMIES[i].speed.y *= FRICTION;
                ENEMIES[i].translate.x += ENEMIES[i].speed.x;
                ENEMIES[i].translate.y += ENEMIES[i].speed.y;

                ENEMIES[i].polar_degrees = polar_degrees_move;
            } else {
                ENEMIES[i].speed.x = 0.0f;
                ENEMIES[i].speed.y = 0.0f;

                if (0.0f < angle) {
                    ENEMIES[i].polar_degrees += TURN;
                    if (360.0f <= ENEMIES[i].polar_degrees) {
                        ENEMIES[i].polar_degrees -= 360.0f;
                    }
                } else {
                    ENEMIES[i].polar_degrees -= TURN;
                    if (ENEMIES[i].polar_degrees < 0.0f) {
                        ENEMIES[i].polar_degrees += 360.0f;
                    }
                }
            }
        }

        const f32 polar_degrees_player = math_polar_degrees((Vec2f){
            .x = PLAYER_CUBE.translate.x - ENEMIES[i].translate.x,
            .y = -(PLAYER_CUBE.translate.z - ENEMIES[i].translate.y),
        });

        f32 angle = polar_degrees_player - ENEMIES[i].polar_degrees;
        {
            const f32 reverse = fabsf(angle - 360.0f);
            angle = fabsf(angle);
            angle = reverse < angle ? reverse : angle;
        }
        ENEMIES[i].player_in_view = FALSE;
        if (angle < FOV_DEGREES) {
            Vec2f line[2] = {
                {PLAYER_CUBE.translate.x, PLAYER_CUBE.translate.z},
                ENEMIES[i].translate,
            };
            for (u32 j = LEN_PLATFORMS; j < LEN_WORLD; ++j) {
                if (intersects(&BOXES[j], line)) {
                    goto skip;
                }
            }
            PLAYER_IN_VIEW = TRUE;
            ENEMIES[i].player_in_view = TRUE;
        }
    skip:
        (void)0;
    }
}

static Geom between(const Geom* l, const Geom* r) {
    return (Geom){
        .translate =
            {
                .x = (l->translate.x / 2.0f) + (r->translate.x / 2.0f),
                .y = (l->translate.y / 2.0f) + (r->translate.y / 2.0f),
                .z = (l->translate.z / 2.0f) + (r->translate.z / 2.0f),
            },
        .scale =
            {
                .x = l->translate.x - r->translate.x,
                .y = l->translate.y - r->translate.y,
                .z = l->translate.z - r->translate.z,
            },
    };
}

void enemy_animate(void) {
    for (u32 i = 0; i < LEN_ENEMIES; ++i) {
        ENEMY_CUBES(i).translate.x = ENEMIES[i].translate.x;
        ENEMY_CUBES(i).translate.z = ENEMIES[i].translate.y;
        ENEMY_SPRITES(i).geom.translate.x = ENEMIES[i].translate.x;
        ENEMY_SPRITES(i).geom.translate.z = ENEMIES[i].translate.y;

        ENEMY_SPRITES(i).col_row.x = OFFSET_SPRITE_COLS;
        if ((HALT < fabsf(ENEMIES[i].speed.x)) ||
            (HALT < fabsf(ENEMIES[i].speed.y)))
        {
            ENEMY_SPRITES(i).col_row.x +=
                SPRITE_COL_INDICES[(time_now() / SPRITE_RATE) % SPRITE_COLS];
        }
        ENEMY_SPRITES(i).col_row.y =
            OFFSET_SPRITE_ROWS +
            SPRITE_DIRECTIONS[DIRECTION(ENEMIES[i].polar_degrees)];

        if (ENEMIES[i].player_in_view) {
            LINES[i] = between(&ENEMY_CUBES(i), &PLAYER_CUBE);
        } else {
            const f32  polar_radians = math_radians(ENEMIES[i].polar_degrees);
            const Geom target = {
                .translate =
                    {
                        .x = ENEMIES[i].translate.x +
                             (RADIUS_LINE * cosf(polar_radians)),
                        .y = TRANSLATE_Y_CUBE,
                        .z = ENEMIES[i].translate.y -
                             (RADIUS_LINE * sinf(polar_radians)),
                    },
                .scale = SCALE_SPRITE,
            };
            LINES[i] = between(&ENEMY_CUBES(i), &target);
        }
        LINES[i].translate.y += TRANSLATE_Y_LINE;
        LINES[i].color = COLOR_LINE;
    }
}
