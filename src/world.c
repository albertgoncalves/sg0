#include "world.h"

#include "geom.h"

#define SCALE_WAYPOINT ((Vec3f){0.325f, 6.0f, 0.325f})

#define COLOR_PLATFORM ((Vec4f){0.2125f, 0.3f, 0.425f, 1.0f})
#define COLOR_WALL     ((Vec4f){0.1875f, 0.2f, 0.3f, 0.675f})

#define ALPHA_WAYPOINT 0.25f

#define WORLD_CUBES(i) CUBES[OFFSET_WORLD + (i)]

#define PLATFORM(x, z, width, depth)   \
    ((Geom){                           \
        .translate = {x, -1.0f, z},    \
        .scale = {width, 1.0f, depth}, \
        .color = COLOR_PLATFORM,       \
    })

#define WALL(x, z, width, depth)       \
    ((Geom){                           \
        .translate = {x, 0.5f, z},     \
        .scale = {width, 2.5f, depth}, \
        .color = COLOR_WALL,           \
    })

#define WAYPOINT(x, z, rgba)       \
    ((Geom){                       \
        .translate = {x, 2.0f, z}, \
        .scale = SCALE_WAYPOINT,   \
        .color = rgba,             \
    })

void world_init(void) {
    LEN_WORLD = 0;

    EXIT_IF(CAP_WORLD < (LEN_WORLD + 2));
    WORLD_CUBES(LEN_WORLD++) = PLATFORM(0.0f, -5.0f, 30.0f, 40.0f);
    WORLD_CUBES(LEN_WORLD++) = PLATFORM(20.5f, 0.0f, 11.0f, 25.0f);

    OFFSET_PLATFORMS = OFFSET_WORLD + LEN_WORLD;

    EXIT_IF(CAP_WORLD < (LEN_WORLD + 15));
    WORLD_CUBES(LEN_WORLD++) = WALL(-7.0f, 5.5f, 10.0f, 14.0f);
    WORLD_CUBES(LEN_WORLD++) = WALL(-7.0f, -14.5f, 10.0f, 15.0f);
    WORLD_CUBES(LEN_WORLD++) = WALL(7.5f, 4.0f, 8.0f, 17.0f);

    WORLD_CUBES(LEN_WORLD++) = WALL(12.0f, -22.0f, 2.0f, 2.0f);
    WORLD_CUBES(LEN_WORLD++) = WALL(4.0f, -22.0f, 2.0f, 2.0f);
    WORLD_CUBES(LEN_WORLD++) = WALL(12.0f, -16.0f, 2.0f, 2.0f);
    WORLD_CUBES(LEN_WORLD++) = WALL(4.0f, -16.0f, 2.0f, 2.0f);
    WORLD_CUBES(LEN_WORLD++) = WALL(12.0f, -10.0f, 2.0f, 2.0f);
    WORLD_CUBES(LEN_WORLD++) = WALL(4.0f, -10.0f, 2.0f, 2.0f);

    WORLD_CUBES(LEN_WORLD++) = WALL(16.0f, -6.0f, 2.0f, 2.0f);
    WORLD_CUBES(LEN_WORLD++) = WALL(22.0f, -6.0f, 2.0f, 2.0f);

    WORLD_CUBES(LEN_WORLD++) = WALL(16.0f, 1.0f, 2.0f, 2.0f);
    WORLD_CUBES(LEN_WORLD++) = WALL(22.0f, 1.0f, 2.0f, 2.0f);

    WORLD_CUBES(LEN_WORLD++) = WALL(16.0f, 8.0f, 2.0f, 2.0f);
    WORLD_CUBES(LEN_WORLD++) = WALL(22.0f, 8.0f, 2.0f, 2.0f);

    OFFSET_WAYPOINTS = OFFSET_WORLD + LEN_WORLD;

    EXIT_IF(CAP_WORLD < (LEN_WORLD + 8));
#define COLOR_WAYPOINT ((Vec4f){0.1f, 1.0f, 0.3f, ALPHA_WAYPOINT})
    WORLD_CUBES(LEN_WORLD++) = WAYPOINT(-13.5f, 13.5f, COLOR_WAYPOINT);  // 0
    WORLD_CUBES(LEN_WORLD++) = WAYPOINT(-13.5f, -23.5f, COLOR_WAYPOINT); // 1
    WORLD_CUBES(LEN_WORLD++) = WAYPOINT(14.0f, -23.5f, COLOR_WAYPOINT);  // 2
    WORLD_CUBES(LEN_WORLD++) = WAYPOINT(14.0f, -11.0f, COLOR_WAYPOINT);  // 3
    WORLD_CUBES(LEN_WORLD++) = WAYPOINT(24.5f, -11.0f, COLOR_WAYPOINT);  // 4
    WORLD_CUBES(LEN_WORLD++) = WAYPOINT(24.5f, 11.0f, COLOR_WAYPOINT);   // 5
    WORLD_CUBES(LEN_WORLD++) = WAYPOINT(13.5f, 11.0f, COLOR_WAYPOINT);   // 6
    WORLD_CUBES(LEN_WORLD++) = WAYPOINT(13.5f, 13.5f, COLOR_WAYPOINT);   // 7
#undef COLOR_WAYPOINT

    EXIT_IF(CAP_WORLD < (LEN_WORLD + 7));
#define COLOR_WAYPOINT ((Vec4f){0.85f, 0.4f, 0.9f, ALPHA_WAYPOINT})
    WORLD_CUBES(LEN_WORLD++) = WAYPOINT(0.6f, 13.5f, COLOR_WAYPOINT);   // 8
    WORLD_CUBES(LEN_WORLD++) = WAYPOINT(0.6f, -4.0f, COLOR_WAYPOINT);   // 9
    WORLD_CUBES(LEN_WORLD++) = WAYPOINT(-13.5f, -4.0f, COLOR_WAYPOINT); // 10
    WORLD_CUBES(LEN_WORLD++) = WAYPOINT(0.6f, -23.5f, COLOR_WAYPOINT);  // 11
    WORLD_CUBES(LEN_WORLD++) = WAYPOINT(8.0f, -23.5f, COLOR_WAYPOINT);  // 12
    WORLD_CUBES(LEN_WORLD++) = WAYPOINT(8.0f, -6.5f, COLOR_WAYPOINT);   // 13
    WORLD_CUBES(LEN_WORLD++) = WAYPOINT(0.6f, -6.5f, COLOR_WAYPOINT);   // 14
#undef COLOR_WAYPOINT

    EXIT_IF(CAP_WORLD < (LEN_WORLD + 2));
#define COLOR_WAYPOINT ((Vec4f){0.9f, 0.8f, 0.4f, ALPHA_WAYPOINT})
    WORLD_CUBES(LEN_WORLD++) = WAYPOINT(19.0f, -11.0f, COLOR_WAYPOINT); // 15
    WORLD_CUBES(LEN_WORLD++) = WAYPOINT(19.0f, 11.0f, COLOR_WAYPOINT);  // 16
#undef COLOR_WAYPOINT

    EXIT_IF(CAP_WORLD < (LEN_WORLD + 3));
#define COLOR_WAYPOINT ((Vec4f){1.0f, 0.45f, 0.25f, ALPHA_WAYPOINT})
    WORLD_CUBES(LEN_WORLD++) = WAYPOINT(13.5f, 10.0f, COLOR_WAYPOINT); // 17
    WORLD_CUBES(LEN_WORLD++) = WAYPOINT(13.5f, -6.5f, COLOR_WAYPOINT); // 18
    WORLD_CUBES(LEN_WORLD++) = WAYPOINT(4.35f, -6.5f, COLOR_WAYPOINT); // 19
#undef COLOR_WAYPOINT

    EXIT_IF(CAP_WORLD < LEN_WORLD);

    LEN_CUBES = CAP_PLAYER + CAP_ENEMIES + LEN_WORLD;
    EXIT_IF(CAP_CUBES < LEN_CUBES);

    for (u32 i = 0; i < LEN_WORLD; ++i) {
        EXIT_IF(CAP_WORLD <= i);
        BOXES[i] = geom_box(&CUBES[OFFSET_WORLD + i]);
    }
}
