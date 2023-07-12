#ifndef WORLD_H
#define WORLD_H

#include "prelude.h"

#define WORLD_GRAVITY 0.000675f
#define WORLD_FLOOR   -2.5f

extern u32 LEN_WORLD;

extern u32 OFFSET_WAYPOINTS;

void world_init(void);

#endif
