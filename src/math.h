#ifndef __MATH_H__
#define __MATH_H__

#include "prelude.h"

#include <math.h>

#define PI 3.14159274f

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

static f32 get_radians(f32 degrees) {
    return (degrees * PI) / 180.0f;
}

static f32 get_degrees(f32 radians) {
    return (radians / PI) * 180.0f;
}

static f32 get_polar_degrees(Vec2f point) {
    const f32 angle = get_degrees(atanf(point.y / point.x));
    if (point.x < 0.0f) {
        return 180.0f + angle;
    }
    if (point.y < 0.0f) {
        return 360.0f + angle;
    }
    return angle;
}

static f32 dot(Vec3f l, Vec3f r) {
    return (l.x * r.x) + (l.y * r.y) + (l.z * r.z);
}

static Vec3f cross(Vec3f l, Vec3f r) {
    return (Vec3f){
        .x = (l.y * r.z) - (l.z * r.y),
        .y = (l.z * r.x) - (l.x * r.z),
        .z = (l.x * r.y) - (l.y * r.x),
    };
}

static f32 length(Vec3f v) {
    return sqrtf(dot(v, v));
}

static Vec3f normalize(Vec3f v) {
    const f32 l = length(v);
    return (Vec3f){
        .x = v.x / l,
        .y = v.y / l,
        .z = v.z / l,
    };
}

static Mat4 look_at(Vec3f view_from, Vec3f view_to, Vec3f up) {
    const Vec3f f = normalize((Vec3f){
        .x = view_to.x - view_from.x,
        .y = view_to.y - view_from.y,
        .z = view_to.z - view_from.z,
    });
    const Vec3f s = normalize(cross(f, up));
    const Vec3f u = cross(s, f);
    return (Mat4){
        .column_row[0][0] = s.x,
        .column_row[0][1] = u.x,
        .column_row[0][2] = -f.x,
        .column_row[0][3] = 0.0f,
        .column_row[1][0] = s.y,
        .column_row[1][1] = u.y,
        .column_row[1][2] = -f.y,
        .column_row[1][3] = 0.0f,
        .column_row[2][0] = s.z,
        .column_row[2][1] = u.z,
        .column_row[2][2] = -f.z,
        .column_row[2][3] = 0.0f,
        .column_row[3][0] = -dot(s, view_from),
        .column_row[3][1] = -dot(u, view_from),
        .column_row[3][2] = dot(f, view_from),
        .column_row[3][3] = 1.0f,
    };
}

static Mat4 perspective(f32 fov_degrees,
                        f32 aspect_ratio,
                        f32 view_near,
                        f32 view_far) {
    const f32 cotangent = 1.0f / tanf(get_radians(fov_degrees) / 2.0f);
    const f32 d = view_near - view_far;
    return (Mat4){
        .column_row[0][0] = cotangent / aspect_ratio,
        .column_row[1][1] = cotangent,
        .column_row[2][3] = -1.0f,
        .column_row[2][2] = (view_near + view_far) / d,
        .column_row[3][2] = (view_near * view_far * 2.0f) / d,
        .column_row[3][3] = 0.0f,
    };
}

#endif
