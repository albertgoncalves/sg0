#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "math.h"

#define GL_GLEXT_PROTOTYPES

#include <GLFW/glfw3.h>

GLFWwindow* graphics_window(void);

void graphics_init(void);
void graphics_free(void);

u32 graphics_cubes(void);
u32 graphics_lines(void);
u32 graphics_sprites(void);

void graphics_update_camera(Vec3f);
void graphics_update_uniforms(void);
void graphics_update_sprites(void);
void graphics_draw(GLFWwindow*, u32, u32, u32);

#endif
