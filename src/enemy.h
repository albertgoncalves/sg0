#ifndef ENEMY_H
#define ENEMY_H

#include "prelude.h"

extern u32 LEN_WAYPOINTS;

extern Bool PLAYER_IN_VIEW;

void enemy_init(void);
void enemy_update(void);
void enemy_animate(void);

#endif
