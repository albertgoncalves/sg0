#ifndef PLAYER_H
#define PLAYER_H

#include "math.h"

#define PLAYER_CUBE CUBES[OFFSET_PLAYER]

void player_init(void);
void player_update(Vec3f);
void player_animate(void);

#endif
