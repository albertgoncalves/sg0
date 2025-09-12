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

u32 LEN_WORLD;

u32 OFFSET_PLAYER;
u32 OFFSET_ENEMIES;

Vec3f PLAYER_SPEED = {0};

Enemy ENEMIES[CAP_ENEMIES];
u32   LEN_ENEMIES;

Bool PLAYER_IN_VIEW;

#define STEPS_PER_FRAME   6
#define FRAMES_PER_SECOND 60
#define NANOS_PER_FRAME   (NANOS_PER_SECOND / FRAMES_PER_SECOND)
#define NANOS_PER_STEP    (NANOS_PER_FRAME / STEPS_PER_FRAME)

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

static void step(GLFWwindow* window, f32 t) {
    EXIT_IF(1.0f < t);

    player_update(input(window), t);
    enemy_update(t);
    graphics_update_camera(PLAYER_CUBE.translate, t);
}

// NOTE: See `https://gafferongames.com/post/fix_your_timestep/`.
static void update(GLFWwindow* window, u64 remaining, u64* steps) {
    glfwPollEvents();

    f64 delta = (f64)remaining;
    for (; NANOS_PER_STEP < delta; delta -= NANOS_PER_STEP) {
        step(window, 1.0f);
        ++(*steps);
    }
    step(window, (f32)(delta / NANOS_PER_STEP));

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
    u64 steps = 0;

    printf("\n\n\n\n\n");
    while (!glfwWindowShouldClose(window)) {
        const u64 now = time_now();
        elapsed += now - prev;

        // NOTE: See `http://www.opengl-tutorial.org/miscellaneous/an-fps-counter/`.
        if (NANOS_PER_SECOND <= elapsed) {
            const f64 nanoseconds_per_frame = ((f64)elapsed) / ((f64)frames);
            printf("\033[5A"
                   "%15.4f ns/f\n"
                   "%15.4f ratio\n"
                   "%15lu frames\n"
                   "%15lu steps\n"
                   "%15.4f steps/frame\n",
                   nanoseconds_per_frame,
                   nanoseconds_per_frame / (NANOS_PER_SECOND / FRAMES_PER_SECOND),
                   frames,
                   steps,
                   ((f64)steps) / ((f64)frames));
            elapsed = 0;
            frames = 0;
            steps = 0;
        }

        update(window, now - prev, &steps);
        graphics_draw(window);

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
    default: {
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
    glfwDestroyWindow(window);
    glfwTerminate();

    return OK;
}
