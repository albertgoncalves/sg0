#ifndef IMAGE_H
#define IMAGE_H

#include "prelude.h"

typedef struct {
    u8* pixels;
    i32 width;
    i32 height;
} Image;

Image image_open(const char*);
void  image_free(Image);

#endif
