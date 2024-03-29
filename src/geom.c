#include "geom.h"

#include <math.h>

Geom geom_lerp(Geom l, Geom r, f32 t) {
    return (Geom){
        .translate = math_lerp_vec3f(l.translate, r.translate, t),
        .scale = math_lerp_vec3f(l.scale, r.scale, t),
        .color = math_lerp_vec4f(l.color, r.color, t),
    };
}

Box geom_box(const Geom* cube) {
    const Vec3f half_scale = (Vec3f){
        .x = cube->scale.x / 2.0f,
        .y = cube->scale.y / 2.0f,
        .z = cube->scale.z / 2.0f,
    };
    return (Box){
        .left_bottom_back.x = cube->translate.x - half_scale.x,
        .left_bottom_back.y = cube->translate.y - half_scale.y,
        .left_bottom_back.z = cube->translate.z - half_scale.z,
        .right_top_front.x = cube->translate.x + half_scale.x,
        .right_top_front.y = cube->translate.y + half_scale.y,
        .right_top_front.z = cube->translate.z + half_scale.z,
    };
}

static Box move(const Box* box, const Vec3f* speed, f32 time) {
    const Vec3f hit_distance = (Vec3f){
        .x = speed->x * time,
        .y = speed->y * time,
        .z = speed->z * time,
    };
    return (Box){
        .left_bottom_back =
            {
                .x = box->left_bottom_back.x + hit_distance.x,
                .y = box->left_bottom_back.y + hit_distance.y,
                .z = box->left_bottom_back.z + hit_distance.z,
            },
        .right_top_front =
            {
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

Collision geom_collision(const Box*   from,
                         const Box*   obstacle,
                         const Vec3f* speed) {
    Vec3f time = {
        .x = -INFINITY,
        .y = -INFINITY,
        .z = -INFINITY,
    };
    if (0.0f < speed->x) {
        time.x = (obstacle->left_bottom_back.x - from->right_top_front.x) /
                 speed->x;
    } else if (speed->x < 0.0f) {
        time.x = (obstacle->right_top_front.x - from->left_bottom_back.x) /
                 speed->x;
    }
    if (0.0f < speed->y) {
        time.y = (obstacle->left_bottom_back.y - from->right_top_front.y) /
                 speed->y;
    } else if (speed->y < 0.0f) {
        time.y = (obstacle->right_top_front.y - from->left_bottom_back.y) /
                 speed->y;
    }
    if (0.0f < speed->z) {
        time.z = (obstacle->left_bottom_back.z - from->right_top_front.z) /
                 speed->z;
    } else if (speed->z < 0.0f) {
        time.z = (obstacle->right_top_front.z - from->left_bottom_back.z) /
                 speed->z;
    }
    Collision collision = {0};
    if ((time.y < time.x) && (time.z < time.x)) {
        if ((time.x < 0.0f) || (1.0f < time.x)) {
            return collision;
        }
        Box to = move(from, speed, time.x);
        if ((to.left_bottom_back.y < obstacle->right_top_front.y) &&
            (obstacle->left_bottom_back.y < to.right_top_front.y) &&
            (to.left_bottom_back.z < obstacle->right_top_front.z) &&
            (obstacle->left_bottom_back.z < to.right_top_front.z))
        {
            collision.time = time.x;
            collision.overlap = overlap_box(&to, obstacle);
            collision.hit = HIT_X;
        }
    } else if ((time.x < time.y) && (time.z < time.y)) {
        if ((time.y < 0.0f) || (1.0f < time.y)) {
            return collision;
        }
        Box to = move(from, speed, time.y);
        if ((to.left_bottom_back.x < obstacle->right_top_front.x) &&
            (obstacle->left_bottom_back.x < to.right_top_front.x) &&
            (to.left_bottom_back.z < obstacle->right_top_front.z) &&
            (obstacle->left_bottom_back.z < to.right_top_front.z))
        {
            collision.time = time.y;
            collision.overlap = overlap_box(&to, obstacle);
            collision.hit = HIT_Y;
        }
    } else {
        if ((time.z < 0.0f) || (1.0f < time.z)) {
            return collision;
        }
        Box to = move(from, speed, time.z);
        if ((to.left_bottom_back.x < obstacle->right_top_front.x) &&
            (obstacle->left_bottom_back.x < to.right_top_front.x) &&
            (to.left_bottom_back.y < obstacle->right_top_front.y) &&
            (obstacle->left_bottom_back.y < to.right_top_front.y))
        {
            collision.time = time.z;
            collision.overlap = overlap_box(&to, obstacle);
            collision.hit = HIT_Z;
        }
    }
    return collision;
}

Bool geom_intersects(const Vec2f l[2], const Vec2f r[2]) {
    const f32 x0 = l[0].x - l[1].x;
    const f32 y0 = l[0].y - l[1].y;

    const f32 x1 = l[0].x - r[0].x;
    const f32 y1 = l[0].y - r[0].y;

    const f32 x2 = r[0].x - r[1].x;
    const f32 y2 = r[0].y - r[1].y;

    const f32 denominator = (x0 * y2) - (y0 * x2);
    if (denominator == 0.0f) {
        return FALSE;
    }
    const f32 t = ((x1 * y2) - (y1 * x2)) / denominator;
    const f32 u = -((x0 * y1) - (y0 * x1)) / denominator;
    return (0.0f <= t) && (t <= 1.0f) && (0.0f <= u) && (u <= 1.0f);
}
