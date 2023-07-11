#include "sprite.h"

#define SPRITE_UPDATE_STEP 250

void sprite_update(void) {
    SPRITE_TIME += SPRITE_UPDATE_STEP;
}
