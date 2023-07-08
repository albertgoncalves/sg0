#ifndef SPRITE_H
#define SPRITE_H

#include "geom.h"

#define PATH_SPRITE_ATLAS "assets/sprite_atlas.png"

#define SPRITE_PLAYER_COLS 5
#define SPRITE_PLAYER_ROWS 8

#define SPRITE_PLAYER_COLS_OFFSET 0
#define SPRITE_PLAYER_ROWS_OFFSET 0

#define SPRITE_ENEMY_COLS 4
#define SPRITE_ENEMY_ROWS 4

#define SPRITE_ENEMY_COLS_OFFSET 5
#define SPRITE_ENEMY_ROWS_OFFSET 0

#define SPRITE_ATLAS_COLS 9
#define SPRITE_ATLAS_ROWS 8

static const u8 SPRITE_DIRECTIONS_PLAYER[SPRITE_PLAYER_ROWS] =
    {3, 4, 0, 7, 6, 5, 1, 2};
static const u8 SPRITE_DIRECTIONS_ENEMY[SPRITE_ENEMY_ROWS] = {2, 1, 3, 0};

#define SPRITE_UPDATE_STEP 250

static u64 SPRITE_TIME = 0;

typedef struct {
    Geom  geom;
    Vec2u col_row;
} Sprite;

static void update_sprites(void) {
    SPRITE_TIME += SPRITE_UPDATE_STEP;
}

#endif
