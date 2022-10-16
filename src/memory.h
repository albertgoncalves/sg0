#ifndef __MEMORY_H__
#define __MEMORY_H__

#include "geom.h"
#include "sprite.h"

#define CAP_CUBES (1 << 6)
static Geom CUBES[CAP_CUBES];
static Box  BOXES[CAP_CUBES];

#define CAP_PLAYER  1
#define CAP_ENEMIES (1 << 4)
#define CAP_WORLD   (CAP_CUBES - (CAP_ENEMIES + CAP_PLAYER))

static u32 LEN_ENEMIES = 0;
static u32 LEN_WORLD = 0;
#define LEN_CUBES (CAP_PLAYER + CAP_ENEMIES + LEN_WORLD)

#define OFFSET_PLAYER  0
#define OFFSET_ENEMIES CAP_PLAYER
#define OFFSET_WORLD   (CAP_PLAYER + CAP_ENEMIES)

#define CAP_LINES (1 << 4)
static Geom LINES[CAP_LINES];
static u32  LEN_LINES = 0;

#define CAP_SPRITES (1 << 4)
static Sprite SPRITES[CAP_SPRITES];
#define LEN_SPRITES (CAP_PLAYER + LEN_ENEMIES)

#endif
