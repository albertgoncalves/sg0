#ifndef MATH_H
#define MATH_H

#include "prelude.h"

typedef struct {
    f32 x, y;
} Vec2f;

typedef struct {
    f32 x, y, z;
} Vec3f;

typedef struct {
    f32 x, y, z, w;
} Vec4f;

typedef struct {
    u8 x, y;
} Vec2u;

typedef struct {
    u8 x, y, z;
} Vec3u;

typedef struct {
    f32 column_row[4][4];
} Mat4;

f32   math_radians(f32);
f32   math_degrees(f32);
f32   math_polar_degrees(Vec2f);
Vec3f math_normalize(Vec3f);
Mat4  math_look_at(Vec3f, Vec3f, Vec3f);
Mat4  math_perspective(f32, f32, f32, f32);

#endif
