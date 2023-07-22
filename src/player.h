#ifndef PLAYER_H
#define PLAYER_H

#include "math.h"

#define PLAYER_CUBE CUBES[OFFSET_PLAYER]

extern Vec3f PLAYER_SPEED;

void player_init(void);
void player_update(Vec3f);
void player_animate(void);

#endif
