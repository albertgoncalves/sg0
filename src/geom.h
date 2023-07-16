#ifndef GEOM_H
#define GEOM_H

#include "math.h"

typedef struct {
    Vec3f translate;
    Vec3f scale;
    Vec4f color;
} Geom;

typedef struct {
    Vec3f position;
    Vec3f normal;
} CubeVertex;

typedef struct {
    Vec3f left_bottom_back;
    Vec3f right_top_front;
} Box;

typedef enum {
    HIT_NONE = 0,
    HIT_X = 1 << 0,
    HIT_Y = 1 << 1,
    HIT_Z = 1 << 2,
} Hit;

typedef struct {
    f32 time;
    f32 overlap;
    Hit hit;
} Collision;

Box       geom_box(const Geom*);
Collision geom_collision(const Box*, const Box*, const Vec3f*);
Geom      geom_between(const Geom*, const Geom*);
Bool      geom_intersects(Vec2f[2], Vec2f[2]);

extern Geom CUBES[CAP_CUBES];
extern u32  LEN_CUBES;

extern Geom LINES[CAP_LINES];
extern u32  LEN_LINES;

extern Box BOXES[CAP_CUBES];

#endif
