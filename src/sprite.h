#ifndef SPRITE_H
#define SPRITE_H

#include "geom.h"

typedef struct {
    Geom  geom;
    Vec2u col_row;
} Sprite;

#define PATH_SPRITE_ATLAS "assets/sprite_atlas.png"

#define SPRITE_ATLAS_COLS 9
#define SPRITE_ATLAS_ROWS 8

void sprite_update(void);

#endif
