#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "geom.h"

#define GL_GLEXT_PROTOTYPES

#include <GLFW/glfw3.h>

typedef struct {
    Geom  geom;
    Vec2u col_row;
} Sprite;

extern Vec3f OFFSET_VIEW;

extern Sprite SPRITES[CAP_SPRITES];
extern u32    LEN_SPRITES;

GLFWwindow* graphics_window(void);

void graphics_init(void);
void graphics_free(void);

u32 graphics_cubes(void);
u32 graphics_lines(void);
u32 graphics_sprites(void);

void graphics_update_camera(Vec3f);
void graphics_update_uniforms(void);
void graphics_update_sprites(void);
void graphics_draw(GLFWwindow*);

#endif
