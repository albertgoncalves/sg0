#ifndef ENEMY_H
#define ENEMY_H

#include "math.h"

typedef struct Waypoint Waypoint;

typedef struct {
    Vec2f           translate;
    Vec2f           speed;
    f32             polar_degrees;
    const Waypoint* waypoint;
    Bool            player_in_view;
} Enemy;

struct Waypoint {
    Vec2f           translate;
    const Waypoint* next;
};

extern Enemy ENEMIES[CAP_ENEMIES];
extern u32   LEN_ENEMIES;

extern u32 LEN_WAYPOINTS;

extern Bool PLAYER_IN_VIEW;

Enemy enemy_lerp(Enemy, Enemy, f32);

void enemy_init(void);
void enemy_update(void);
void enemy_animate(void);

#endif
