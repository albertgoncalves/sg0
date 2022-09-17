#include "image.h"

#define STBI_ASSERT(condition) EXIT_IF(!(condition))

#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_PNG

#include "stb_image.h"

#undef STB_IMAGE_IMPLEMENTATION
#undef STBI_ONLY_PNG

Image image_rgba_from_path(const char* path) {
    i32 width;
    i32 height;
    u8* pixels = stbi_load(path, &width, &height, NULL, STBI_rgb_alpha);
    return (Image){
        .pixels = pixels,
        .width = width,
        .height = height,
    };
}

void image_free(Image image) {
    stbi_image_free(image.pixels);
}
