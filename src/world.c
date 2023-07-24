#include "world.h"

#include "geom.h"

#define SCALE_WAYPOINT ((Vec3f){0.325f, 6.0f, 0.325f})

#define COLOR_PLATFORM ((Vec4f){0.2125f, 0.3f, 0.425f, 1.0f})
#define COLOR_WALL     ((Vec4f){0.1875f, 0.2f, 0.3f, 0.675f})

#define WAYPOINT_ALPHA 0.25f

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

    WORLD_CUBES(LEN_WORLD++) = PLATFORM(0.0f, -5.0f, 30.0f, 40.0f);
    WORLD_CUBES(LEN_WORLD++) = PLATFORM(16.0f, 0.0f, 20.0f, 25.0f);

    OFFSET_PLATFORMS = OFFSET_WORLD + LEN_WORLD;

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

#define WAYPOINT_COLOR ((Vec4f){0.1f, 1.0f, 0.3f, WAYPOINT_ALPHA})
    WORLD_CUBES(LEN_WORLD++) = WAYPOINT(-13.5f, 13.5f, WAYPOINT_COLOR);  // 0
    WORLD_CUBES(LEN_WORLD++) = WAYPOINT(-13.5f, -23.5f, WAYPOINT_COLOR); // 1
    WORLD_CUBES(LEN_WORLD++) = WAYPOINT(14.0f, -23.5f, WAYPOINT_COLOR);  // 2
    WORLD_CUBES(LEN_WORLD++) = WAYPOINT(14.0f, -11.0f, WAYPOINT_COLOR);  // 3
    WORLD_CUBES(LEN_WORLD++) = WAYPOINT(24.5f, -11.0f, WAYPOINT_COLOR);  // 4
    WORLD_CUBES(LEN_WORLD++) = WAYPOINT(24.5f, 11.0f, WAYPOINT_COLOR);   // 5
    WORLD_CUBES(LEN_WORLD++) = WAYPOINT(13.5f, 11.0f, WAYPOINT_COLOR);   // 6
    WORLD_CUBES(LEN_WORLD++) = WAYPOINT(13.5f, 13.5f, WAYPOINT_COLOR);   // 7
#undef WAYPOINT_COLOR

#define WAYPOINT_COLOR ((Vec4f){0.85f, 0.4f, 0.9f, WAYPOINT_ALPHA})
    WORLD_CUBES(LEN_WORLD++) = WAYPOINT(0.6f, 13.5f, WAYPOINT_COLOR);   // 8
    WORLD_CUBES(LEN_WORLD++) = WAYPOINT(0.6f, -4.0f, WAYPOINT_COLOR);   // 9
    WORLD_CUBES(LEN_WORLD++) = WAYPOINT(-13.5f, -4.0f, WAYPOINT_COLOR); // 10
    WORLD_CUBES(LEN_WORLD++) = WAYPOINT(0.6f, -23.5f, WAYPOINT_COLOR);  // 11
    WORLD_CUBES(LEN_WORLD++) = WAYPOINT(8.0f, -23.5f, WAYPOINT_COLOR);  // 12
    WORLD_CUBES(LEN_WORLD++) = WAYPOINT(8.0f, -6.5f, WAYPOINT_COLOR);   // 13
    WORLD_CUBES(LEN_WORLD++) = WAYPOINT(0.6f, -6.5f, WAYPOINT_COLOR);   // 14
#undef WAYPOINT_COLOR

#define WAYPOINT_COLOR ((Vec4f){0.9f, 0.8f, 0.4f, WAYPOINT_ALPHA})
    WORLD_CUBES(LEN_WORLD++) = WAYPOINT(19.0f, -11.0f, WAYPOINT_COLOR); // 15
    WORLD_CUBES(LEN_WORLD++) = WAYPOINT(19.0f, 11.0f, WAYPOINT_COLOR);  // 16
#undef WAYPOINT_COLOR

#define WAYPOINT_COLOR ((Vec4f){1.0f, 0.45f, 0.25f, WAYPOINT_ALPHA})
    WORLD_CUBES(LEN_WORLD++) = WAYPOINT(13.5f, 10.0f, WAYPOINT_COLOR); // 17
    WORLD_CUBES(LEN_WORLD++) = WAYPOINT(13.5f, -6.5f, WAYPOINT_COLOR); // 18
    WORLD_CUBES(LEN_WORLD++) = WAYPOINT(4.35f, -6.5f, WAYPOINT_COLOR); // 19
#undef WAYPOINT_COLOR

    EXIT_IF(CAP_WORLD < LEN_WORLD);

    LEN_CUBES = CAP_PLAYER + CAP_ENEMIES + LEN_WORLD;
    EXIT_IF(CAP_CUBES < LEN_CUBES);

    for (u32 i = 0; i < LEN_WORLD; ++i) {
        BOXES[i] = geom_box(&CUBES[OFFSET_WORLD + i]);
    }
}
