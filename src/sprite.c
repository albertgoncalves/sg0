#include "sprite.h"

#define SPRITE_UPDATE_STEP 250

extern u64 SPRITE_TIME;

void sprite_update(void) {
    SPRITE_TIME += SPRITE_UPDATE_STEP;
}
