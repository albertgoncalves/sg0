#ifndef IMAGE_H
#define IMAGE_H

#include "prelude.h"

typedef struct {
    u8* pixels;
    i32 width;
    i32 height;
} Image;

Image image_rgba_from_path(const char*);
void  image_free(Image);

#endif
