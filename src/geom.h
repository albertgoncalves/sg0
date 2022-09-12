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
    Vec2f left_bottom;
    Vec2f right_top;
} Rect;

typedef struct {
    f32  time;
    Bool hit;
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

static void set_rect_from_cube_xz(const Cube* cube, Rect* rect) {
    Vec2f half_scale = (Vec2f){
        .x = cube->scale.x / 2.0f,
        .y = cube->scale.z / 2.0f,
    };
    rect->left_bottom.x = cube->translate.x - half_scale.x;
    rect->left_bottom.y = cube->translate.z - half_scale.y;
    rect->right_top.x = cube->translate.x + half_scale.x;
    rect->right_top.y = cube->translate.z + half_scale.y;
}

static Collision get_rect_collision(const Rect* move_from,
                                    const Rect* obstacle,
                                    Vec2f*      speed) {
    Vec2f time = {
        .x = -INFINITY,
        .y = -INFINITY,
    };
    if (0.0f < speed->x) {
        time.x = (obstacle->left_bottom.x - move_from->right_top.x) / speed->x;
    } else if (speed->x < 0.0f) {
        time.x = (obstacle->right_top.x - move_from->left_bottom.x) / speed->x;
    }
    if (0.0f < speed->y) {
        time.y = (obstacle->left_bottom.y - move_from->right_top.y) / speed->y;
    } else if (speed->y < 0.0f) {
        time.y = (obstacle->right_top.y - move_from->left_bottom.y) / speed->y;
    }
    Collision collision = {0};
    if (time.y < time.x) {
        if ((time.x < 0.0f) || (1.0f < time.x)) {
            return collision;
        }
        Vec2f hit_distance = (Vec2f){
            .x = speed->x * time.x,
            .y = speed->y * time.x,
        };
        Rect move_to = (Rect){
            .left_bottom =
                (Vec2f){
                    .x = move_from->left_bottom.x + hit_distance.x,
                    .y = move_from->left_bottom.y + hit_distance.y,
                },
            .right_top =
                (Vec2f){
                    .x = move_from->right_top.x + hit_distance.x,
                    .y = move_from->right_top.y + hit_distance.y,
                },
        };
        collision.hit = ((move_to.left_bottom.y < obstacle->right_top.y) &&
                         (obstacle->left_bottom.y < move_to.right_top.y));
    } else if (time.x < time.y) {
        if ((time.y < 0.0f) || (1.0f < time.y)) {
            return collision;
        }
        Vec2f hit_distance = (Vec2f){
            .x = speed->x * time.y,
            .y = speed->y * time.y,
        };
        Rect move_to = (Rect){
            .left_bottom =
                (Vec2f){
                    .x = move_from->left_bottom.x + hit_distance.x,
                    .y = move_from->left_bottom.y + hit_distance.y,
                },
            .right_top =
                (Vec2f){
                    .x = move_from->right_top.x + hit_distance.x,
                    .y = move_from->right_top.y + hit_distance.y,
                },
        };
        collision.hit = ((move_to.left_bottom.x < obstacle->right_top.x) &&
                         (obstacle->left_bottom.x < move_to.right_top.x));
    }
    return collision;
}

#endif
