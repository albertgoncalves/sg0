#include "enemy.h"
#include "graphics.h"
#include "pcg_rng.h"
#include "player.h"
#include "string.h"
#include "time.h"
#include "world.h"

#include <string.h>

char BUFFER[CAP_BUFFER];
u32  LEN_BUFFER = 0;

Vec3f OFFSET_VIEW = {0};

Geom CUBES[CAP_CUBES];
u32  LEN_CUBES;

Geom LINES[CAP_LINES];
u32  LEN_LINES;

Sprite SPRITES[CAP_SPRITES];
u32    LEN_SPRITES;

Box BOXES[CAP_WORLD];

u32 LEN_PLATFORMS;
u32 LEN_WORLD;

u32 OFFSET_PLAYER;
u32 OFFSET_ENEMIES;

Vec3f PLAYER_SPEED = {0};

Enemy ENEMIES[CAP_ENEMIES];
u32   LEN_ENEMIES;

Bool PLAYER_IN_VIEW;

static Enemy PREVIOUS_ENEMIES[CAP_ENEMIES];

static Geom  PREVIOUS_PLAYER_CUBE = {0};
static Vec3f PREVIOUS_PLAYER_SPEED = {0};
static Vec3f PREVIOUS_OFFSET_VIEW = {0};
static Bool  PREVIOUS_PLAYER_IN_VIEW = FALSE;

#define STEPS_PER_FRAME   6
#define FRAMES_PER_SECOND 60
#ifdef VSYNC
    #define NANOS_PER_FRAME (NANOS_PER_SECOND / FRAMES_PER_SECOND)
#else
    #define NANOS_PER_FRAME ((NANOS_PER_SECOND / FRAMES_PER_SECOND) - 175000)
#endif
#define NANOS_PER_STEP (NANOS_PER_FRAME / STEPS_PER_FRAME)

static Vec3f input(GLFWwindow* window) {
    Vec3f move = {0};
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        move.z -= 1.0f;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        move.z += 1.0f;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        move.x -= 1.0f;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        move.x += 1.0f;
    }
    if ((move.x == 0.0f) && (move.z == 0.0f)) {
        return move;
    }
    return math_normalize(move);
}

static void init(void) {
    world_init();
    player_init();
    enemy_init();

    LEN_CUBES = LEN_WORLD + CAP_PLAYER + LEN_ENEMIES;
    EXIT_IF(CAP_CUBES < LEN_CUBES);

    LEN_SPRITES = CAP_PLAYER + LEN_ENEMIES;
    EXIT_IF(CAP_SPRITES < LEN_SPRITES);

    LEN_LINES = LEN_ENEMIES;
    EXIT_IF(CAP_LINES < LEN_LINES);
}

static void step(GLFWwindow* window) {
    glfwPollEvents();
    player_update(input(window));
    enemy_update();
    graphics_update_camera(PLAYER_CUBE.translate);
}

// NOTE: See `https://gafferongames.com/post/fix_your_timestep/`.
static void update(GLFWwindow* window, u64 remaining) {
#ifndef VSYNC
    remaining = NANOS_PER_FRAME;
#endif
    for (; NANOS_PER_STEP < remaining; remaining -= NANOS_PER_STEP) {
#ifndef VSYNC
        const u64 now = time_now();
#endif
        step(window);
#ifndef VSYNC
        time_sleep(now + NANOS_PER_STEP);
#endif
    }

    PREVIOUS_OFFSET_VIEW = OFFSET_VIEW;

    PREVIOUS_PLAYER_CUBE = PLAYER_CUBE;
    PREVIOUS_PLAYER_SPEED = PLAYER_SPEED;

    memcpy(PREVIOUS_ENEMIES, ENEMIES, sizeof(Enemy) * LEN_ENEMIES);
    PREVIOUS_PLAYER_IN_VIEW = PLAYER_IN_VIEW;

    step(window);
    {
        const f32 blend = ((f32)remaining) / ((f32)NANOS_PER_STEP);
        EXIT_IF(blend <= 0.0f);
        EXIT_IF(1.0f < blend);

        OFFSET_VIEW =
            math_lerp_vec3f(PREVIOUS_OFFSET_VIEW, OFFSET_VIEW, blend);

        PLAYER_CUBE = geom_lerp(PREVIOUS_PLAYER_CUBE, PLAYER_CUBE, blend);
        PLAYER_SPEED =
            math_lerp_vec3f(PREVIOUS_PLAYER_SPEED, PLAYER_SPEED, blend);

        for (u32 i = 0; i < LEN_ENEMIES; ++i) {
            ENEMIES[i] = enemy_lerp(PREVIOUS_ENEMIES[i], ENEMIES[i], blend);
        }
        PLAYER_IN_VIEW =
            math_lerp_bool(PREVIOUS_PLAYER_IN_VIEW, PLAYER_IN_VIEW, blend);
    }

#if 0
    if (PLAYER_IN_VIEW) {
        init();
    }
#endif

    player_animate();
    enemy_animate();
    graphics_update_uniforms();
}

static void loop(GLFWwindow* window) {
    u64 prev = time_now();
    u64 elapsed = 0;
    u64 frames = 0;

    printf("\n\n\n");
    while (!glfwWindowShouldClose(window)) {
        const u64 now = time_now();
        elapsed += now - prev;

        // NOTE: See `http://www.opengl-tutorial.org/miscellaneous/an-fps-counter/`.
        if (NANOS_PER_SECOND <= elapsed) {
            const f64 nanoseconds_per_frame = ((f64)elapsed) / ((f64)frames);
            printf("\033[3A"
                   "%9.0f ns/f\n"
                   "%9.4f ratio\n"
                   "%9lu frames\n",
                   nanoseconds_per_frame,
                   nanoseconds_per_frame /
                       (NANOS_PER_SECOND / FRAMES_PER_SECOND),
                   frames);
            elapsed -= NANOS_PER_SECOND;
            frames = 0;
        }

        update(window, now - prev);
        graphics_draw(window);
#ifndef VSYNC
        time_sleep(now + NANOS_PER_FRAME);
#endif

        prev = now;
        ++frames;
    }
}

static void callback(GLFWwindow* window, i32 key, i32, i32 action, i32) {
    if (action != GLFW_PRESS) {
        return;
    }
    switch (key) {
    case GLFW_KEY_ESCAPE: {
        glfwSetWindowShouldClose(window, TRUE);
        break;
    }
    case GLFW_KEY_R: {
        init();
        break;
    }
    }
}

i32 main(void) {
    printf("GLFW version : %s\n", glfwGetVersionString());
    GLFWwindow* window = graphics_window();
    glfwSetKeyCallback(window, callback);
    printf("GL_VERSION   : %s\n"
           "GL_VENDOR    : %s\n"
           "GL_RENDERER  : %s\n"
           "\n",
           glGetString(GL_VERSION),
           glGetString(GL_VENDOR),
           glGetString(GL_RENDERER));
    graphics_init();

    pcg_rng_seed(time_now(), 1);
    init();

    loop(window);

    graphics_free();
    glfwTerminate();
    return OK;
}
