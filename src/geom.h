#ifndef GEOM_H
#define GEOM_H

#include "math.h"

typedef struct {
    Vec3f translate;
    Vec3f scale;
    Vec4f color;
} Geom;

STATIC_ASSERT(sizeof(Geom) == 40);

typedef struct {
    Vec3f position;
    Vec3f normal;
} CubeVertex;

typedef struct {
    Vec3f left_bottom_back;
    Vec3f right_top_front;
} Box;

STATIC_ASSERT(sizeof(Box) == 24);

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

STATIC_ASSERT(sizeof(Collision) == 12);

Box       geom_box(const Geom*);
Collision geom_collision(const Box*, const Box*, const Vec3f*);
Bool      geom_intersects(const Vec2f[2], const Vec2f[2], f32*);

extern Geom CUBES[CAP_CUBES];
extern u32  LEN_CUBES;

extern Geom LINES[CAP_LINES];
extern u32  LEN_LINES;

extern Box BOXES[CAP_WORLD];

#endif
