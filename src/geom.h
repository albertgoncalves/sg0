#ifndef __GEOM_H__
#define __GEOM_H__

#include "math.h"

typedef struct {
    Vec3f position;
    Vec3f normal;
} Vertex;

typedef struct {
    Vec3f translate;
    Vec3f scale;
    Vec4f color;
} Cube;

typedef struct {
    Vec3f left_bottom_back;
    Vec3f right_top_forward;
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

static const Vertex VERTICES[] = {
    {{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}},
    {{0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}},
    {{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}},
    {{-0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}},
    {{-0.5f, -0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
    {{0.5f, -0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
    {{0.5f, 0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
    {{-0.5f, 0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
    {{-0.5f, 0.5f, 0.5f}, {-1.0f, 0.0f, 0.0f}},
    {{-0.5f, 0.5f, -0.5f}, {-1.0f, 0.0f, 0.0f}},
    {{-0.5f, -0.5f, -0.5f}, {-1.0f, 0.0f, 0.0f}},
    {{-0.5f, -0.5f, 0.5f}, {-1.0f, 0.0f, 0.0f}},
    {{0.5f, 0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}},
    {{0.5f, 0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
    {{0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
    {{0.5f, -0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}},
    {{-0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f}},
    {{0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f}},
    {{0.5f, -0.5f, 0.5f}, {0.0f, -1.0f, 0.0f}},
    {{-0.5f, -0.5f, 0.5f}, {0.0f, -1.0f, 0.0f}},
    {{-0.5f, 0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
    {{0.5f, 0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
    {{0.5f, 0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
    {{-0.5f, 0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
};

static const Vec3u INDICES[] = {
    {0, 1, 2},
    {2, 3, 0},
    {4, 5, 6},
    {6, 7, 4},
    {8, 9, 10},
    {10, 11, 8},
    {12, 13, 14},
    {14, 15, 12},
    {16, 17, 18},
    {18, 19, 16},
    {20, 21, 22},
    {22, 23, 20},
};

static void set_box_from_cube(const Cube* cube, Box* box) {
    Vec3f half_scale = (Vec3f){
        .x = cube->scale.x / 2.0f,
        .y = cube->scale.y / 2.0f,
        .z = cube->scale.z / 2.0f,
    };
    box->left_bottom_back.x = cube->translate.x - half_scale.x;
    box->left_bottom_back.y = cube->translate.y - half_scale.y;
    box->left_bottom_back.z = cube->translate.z - half_scale.z;
    box->right_top_forward.x = cube->translate.x + half_scale.x;
    box->right_top_forward.y = cube->translate.y + half_scale.y;
    box->right_top_forward.z = cube->translate.z + half_scale.z;
}

static Box get_move_to(const Box* move_from, const Vec3f* speed, f32 time) {
    Vec3f hit_distance = (Vec3f){
        .x = speed->x * time,
        .y = speed->y * time,
        .z = speed->z * time,
    };
    return (Box){
        .left_bottom_back =
            (Vec3f){
                .x = move_from->left_bottom_back.x + hit_distance.x,
                .y = move_from->left_bottom_back.y + hit_distance.y,
                .z = move_from->left_bottom_back.z + hit_distance.z,
            },
        .right_top_forward =
            (Vec3f){
                .x = move_from->right_top_forward.x + hit_distance.x,
                .y = move_from->right_top_forward.y + hit_distance.y,
                .z = move_from->right_top_forward.z + hit_distance.z,
            },
    };
}

static f32 get_overlap_segment(f32 l0, f32 r0, f32 l1, f32 r1) {
    f32 a;
    if (l0 < l1) {
        a = l1;
    } else {
        a = l0;
    }
    f32 b;
    if (r0 < r1) {
        b = r0;
    } else {
        b = r1;
    }
    const f32 c = b - a;
    return c < 0.0f ? 0.0f : c;
}

static f32 get_overlap_box(const Box* l, const Box* r) {
    return get_overlap_segment(l->left_bottom_back.x,
                               l->right_top_forward.x,
                               r->left_bottom_back.x,
                               r->right_top_forward.x) +
           get_overlap_segment(l->left_bottom_back.y,
                               l->right_top_forward.y,
                               r->left_bottom_back.y,
                               r->right_top_forward.y) +
           get_overlap_segment(l->left_bottom_back.z,
                               l->right_top_forward.z,
                               r->left_bottom_back.z,
                               r->right_top_forward.z);
}

static Collision get_box_collision(const Box*   move_from,
                                   const Box*   obstacle,
                                   const Vec3f* speed) {
    Vec3f time = {
        .x = -INFINITY,
        .y = -INFINITY,
        .z = -INFINITY,
    };
    if (0.0f < speed->x) {
        time.x =
            (obstacle->left_bottom_back.x - move_from->right_top_forward.x) /
            speed->x;
    } else if (speed->x < 0.0f) {
        time.x =
            (obstacle->right_top_forward.x - move_from->left_bottom_back.x) /
            speed->x;
    }
    if (0.0f < speed->y) {
        time.y =
            (obstacle->left_bottom_back.y - move_from->right_top_forward.y) /
            speed->y;
    } else if (speed->y < 0.0f) {
        time.y =
            (obstacle->right_top_forward.y - move_from->left_bottom_back.y) /
            speed->y;
    }
    if (0.0f < speed->z) {
        time.z =
            (obstacle->left_bottom_back.z - move_from->right_top_forward.z) /
            speed->z;
    } else if (speed->z < 0.0f) {
        time.z =
            (obstacle->right_top_forward.z - move_from->left_bottom_back.z) /
            speed->z;
    }
    Collision collision = {0};
    if ((time.y < time.x) && (time.z < time.x)) {
        if ((time.x < 0.0f) || (1.0f < time.x)) {
            return collision;
        }
        Box move_to = get_move_to(move_from, speed, time.x);
        if ((move_to.left_bottom_back.y < obstacle->right_top_forward.y) &&
            (obstacle->left_bottom_back.y < move_to.right_top_forward.y) &&
            (move_to.left_bottom_back.z < obstacle->right_top_forward.z) &&
            (obstacle->left_bottom_back.z < move_to.right_top_forward.z))
        {
            collision.overlap = get_overlap_box(&move_to, obstacle);
            collision.hit = HIT_X;
        }
    } else if ((time.x < time.y) && (time.z < time.y)) {
        if ((time.y < 0.0f) || (1.0f < time.y)) {
            return collision;
        }
        Box move_to = get_move_to(move_from, speed, time.y);
        if ((move_to.left_bottom_back.x < obstacle->right_top_forward.x) &&
            (obstacle->left_bottom_back.x < move_to.right_top_forward.x) &&
            (move_to.left_bottom_back.z < obstacle->right_top_forward.z) &&
            (obstacle->left_bottom_back.z < move_to.right_top_forward.z))
        {
            collision.overlap = get_overlap_box(&move_to, obstacle);
            collision.hit = HIT_Y;
        }
    } else {
        if ((time.z < 0.0f) || (1.0f < time.z)) {
            return collision;
        }
        Box move_to = get_move_to(move_from, speed, time.z);
        if ((move_to.left_bottom_back.x < obstacle->right_top_forward.x) &&
            (obstacle->left_bottom_back.x < move_to.right_top_forward.x) &&
            (move_to.left_bottom_back.y < obstacle->right_top_forward.y) &&
            (obstacle->left_bottom_back.y < move_to.right_top_forward.y))
        {
            collision.overlap = get_overlap_box(&move_to, obstacle);
            collision.hit = HIT_Z;
        }
    }
    return collision;
}

#endif
