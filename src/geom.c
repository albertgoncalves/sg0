#include "geom.h"

#include <math.h>

void geom_cube_into_box(const Geom* cube, Box* box) {
    const Vec3f half_scale = (Vec3f){
        .x = cube->scale.x / 2.0f,
        .y = cube->scale.y / 2.0f,
        .z = cube->scale.z / 2.0f,
    };
    box->left_bottom_back.x = cube->translate.x - half_scale.x;
    box->left_bottom_back.y = cube->translate.y - half_scale.y;
    box->left_bottom_back.z = cube->translate.z - half_scale.z;
    box->right_top_front.x = cube->translate.x + half_scale.x;
    box->right_top_front.y = cube->translate.y + half_scale.y;
    box->right_top_front.z = cube->translate.z + half_scale.z;
}

static Box move_box(const Box* box, const Vec3f* speed, f32 time) {
    const Vec3f hit_distance = (Vec3f){
        .x = speed->x * time,
        .y = speed->y * time,
        .z = speed->z * time,
    };
    return (Box){
        .left_bottom_back =
            (Vec3f){
                .x = box->left_bottom_back.x + hit_distance.x,
                .y = box->left_bottom_back.y + hit_distance.y,
                .z = box->left_bottom_back.z + hit_distance.z,
            },
        .right_top_front =
            (Vec3f){
                .x = box->right_top_front.x + hit_distance.x,
                .y = box->right_top_front.y + hit_distance.y,
                .z = box->right_top_front.z + hit_distance.z,
            },
    };
}

static f32 overlap_segment(f32 l0, f32 r0, f32 l1, f32 r1) {
    const f32 a = l0 < l1 ? l1 : l0;
    const f32 b = r0 < r1 ? r0 : r1;
    const f32 c = b - a;
    return c < 0.0f ? 0.0f : c;
}

static f32 overlap_box(const Box* l, const Box* r) {
    return overlap_segment(l->left_bottom_back.x,
                           l->right_top_front.x,
                           r->left_bottom_back.x,
                           r->right_top_front.x) +
           overlap_segment(l->left_bottom_back.y,
                           l->right_top_front.y,
                           r->left_bottom_back.y,
                           r->right_top_front.y) +
           overlap_segment(l->left_bottom_back.z,
                           l->right_top_front.z,
                           r->left_bottom_back.z,
                           r->right_top_front.z);
}

Collision geom_collision(const Box*   move_from,
                         const Box*   obstacle,
                         const Vec3f* speed) {
    Vec3f time = {
        .x = -INFINITY,
        .y = -INFINITY,
        .z = -INFINITY,
    };
    if (0.0f < speed->x) {
        time.x =
            (obstacle->left_bottom_back.x - move_from->right_top_front.x) /
            speed->x;
    } else if (speed->x < 0.0f) {
        time.x =
            (obstacle->right_top_front.x - move_from->left_bottom_back.x) /
            speed->x;
    }
    if (0.0f < speed->y) {
        time.y =
            (obstacle->left_bottom_back.y - move_from->right_top_front.y) /
            speed->y;
    } else if (speed->y < 0.0f) {
        time.y =
            (obstacle->right_top_front.y - move_from->left_bottom_back.y) /
            speed->y;
    }
    if (0.0f < speed->z) {
        time.z =
            (obstacle->left_bottom_back.z - move_from->right_top_front.z) /
            speed->z;
    } else if (speed->z < 0.0f) {
        time.z =
            (obstacle->right_top_front.z - move_from->left_bottom_back.z) /
            speed->z;
    }
    Collision collision = {0};
    if ((time.y < time.x) && (time.z < time.x)) {
        if ((time.x < 0.0f) || (1.0f < time.x)) {
            return collision;
        }
        Box move_to = move_box(move_from, speed, time.x);
        if ((move_to.left_bottom_back.y < obstacle->right_top_front.y) &&
            (obstacle->left_bottom_back.y < move_to.right_top_front.y) &&
            (move_to.left_bottom_back.z < obstacle->right_top_front.z) &&
            (obstacle->left_bottom_back.z < move_to.right_top_front.z))
        {
            collision.time = time.x;
            collision.overlap = overlap_box(&move_to, obstacle);
            collision.hit = HIT_X;
        }
    } else if ((time.x < time.y) && (time.z < time.y)) {
        if ((time.y < 0.0f) || (1.0f < time.y)) {
            return collision;
        }
        Box move_to = move_box(move_from, speed, time.y);
        if ((move_to.left_bottom_back.x < obstacle->right_top_front.x) &&
            (obstacle->left_bottom_back.x < move_to.right_top_front.x) &&
            (move_to.left_bottom_back.z < obstacle->right_top_front.z) &&
            (obstacle->left_bottom_back.z < move_to.right_top_front.z))
        {
            collision.time = time.y;
            collision.overlap = overlap_box(&move_to, obstacle);
            collision.hit = HIT_Y;
        }
    } else {
        if ((time.z < 0.0f) || (1.0f < time.z)) {
            return collision;
        }
        Box move_to = move_box(move_from, speed, time.z);
        if ((move_to.left_bottom_back.x < obstacle->right_top_front.x) &&
            (obstacle->left_bottom_back.x < move_to.right_top_front.x) &&
            (move_to.left_bottom_back.y < obstacle->right_top_front.y) &&
            (obstacle->left_bottom_back.y < move_to.right_top_front.y))
        {
            collision.time = time.z;
            collision.overlap = overlap_box(&move_to, obstacle);
            collision.hit = HIT_Z;
        }
    }
    return collision;
}

Geom geom_between(const Geom* l, const Geom* r) {
    return (Geom){
        .translate =
            {
                .x = (l->translate.x / 2.0f) + (r->translate.x / 2.0f),
                .y = (l->translate.y / 2.0f) + (r->translate.y / 2.0f),
                .z = (l->translate.z / 2.0f) + (r->translate.z / 2.0f),
            },
        .scale =
            {
                .x = l->translate.x - r->translate.x,
                .y = l->translate.y - r->translate.y,
                .z = l->translate.z - r->translate.z,
            },
        .color =
            {
                .x = (l->color.x / 2.0f) + (r->color.x / 2.0f),
                .y = (l->color.y / 2.0f) + (r->color.y / 2.0f),
                .z = (l->color.z / 2.0f) + (r->color.z / 2.0f),
                .w = (l->color.w / 2.0f) + (r->color.w / 2.0f),
            },
    };
}
