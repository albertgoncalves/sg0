#ifndef MATH_H
#define MATH_H

#include "prelude.h"

typedef struct {
    f32 x, y;
} Vec2f;

STATIC_ASSERT(sizeof(Vec2f) == 8);

typedef struct {
    f32 x, y, z;
} Vec3f;

STATIC_ASSERT(sizeof(Vec3f) == 12);

typedef struct {
    f32 x, y, z, w;
} Vec4f;

STATIC_ASSERT(sizeof(Vec4f) == 16);

typedef struct {
    u8 x, y;
} Vec2u;

typedef struct {
    u8 x, y, z;
} Vec3u;

typedef struct {
    f32 column_row[4][4];
} Mat4;

Bool        math_lerp_bool(Bool, Bool, f32);
const void* math_lerp_pointer(const void*, const void*, f32);
f32         math_lerp_f32(f32, f32, f32);
Vec2f       math_lerp_vec2f(Vec2f, Vec2f, f32);
Vec3f       math_lerp_vec3f(Vec3f, Vec3f, f32);
Vec4f       math_lerp_vec4f(Vec4f, Vec4f, f32);

f32 math_radians(f32);
f32 math_degrees(f32);
f32 math_polar_degrees(Vec2f);

Vec3f math_normalize(Vec3f);

Mat4 math_look_at(Vec3f, Vec3f, Vec3f);
Mat4 math_perspective(f32, f32, f32, f32);

#endif
