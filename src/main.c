#include "enemy.h"
#include "graphics.h"
#include "pcg_rng.h"
#include "player.h"
#include "sprite.h"
#include "string.h"
#include "time.h"
#include "world.h"

#include <string.h>

char BUFFER[CAP_BUFFER];
u32  LEN_BUFFER = 0;

Vec3f VIEW_OFFSET = {0};

Geom CUBES[CAP_CUBES];
u32  LEN_CUBES = CAP_PLAYER + CAP_ENEMIES;

Geom LINES[CAP_LINES];
u32  LEN_LINES;

Sprite SPRITES[CAP_SPRITES];
u32    LEN_SPRITES = CAP_PLAYER;

// NOTE: We only need `Box` memory for the world geometry. Would be nice to
// shrink this.
Box BOXES[CAP_CUBES];

u32 LEN_WORLD;
u32 LEN_WAYPOINTS;

u32 OFFSET_PLATFORMS;
u32 OFFSET_WAYPOINTS;

Vec3f PLAYER_SPEED = {0};

Enemy ENEMIES[CAP_ENEMIES];
u32   LEN_ENEMIES;

u64 SPRITE_TIME = 0;

Bool PLAYER_IN_VIEW = FALSE;

static Enemy PREVIOUS_ENEMIES[CAP_ENEMIES];

static Geom  PREVIOUS_PLAYER_CUBE = {0};
static Vec3f PREVIOUS_PLAYER_SPEED = {0};
static Vec3f PREVIOUS_VIEW_OFFSET = {0};
static Bool  PREVIOUS_PLAYER_IN_VIEW = FALSE;

#define STEPS_PER_FRAME 6
#define FRAME_DURATION  (NANO_PER_SECOND / (60 + 1))
#define STEP_DURATION   (FRAME_DURATION / STEPS_PER_FRAME)

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
}

static void step(GLFWwindow* window) {
    glfwPollEvents();
    player_update(input(window));
    enemy_update();
    sprite_update();
    graphics_update_camera(PLAYER_CUBE.translate);
}

// NOTE: See `https://gafferongames.com/post/fix_your_timestep/`.
static void update(GLFWwindow* window) {
    u64 delta = FRAME_DURATION;
    u64 prev = time_nanoseconds();
    for (; STEP_DURATION < delta; delta -= STEP_DURATION) {
        step(window);

        const u64 now = time_nanoseconds();
        const u64 elapsed = now - prev;
        if (elapsed < STEP_DURATION) {
            const u64 remaining = STEP_DURATION - elapsed;
            EXIT_IF(usleep((u32)(remaining / NANO_PER_MICRO)));
        }
        prev = now;
    }

    PREVIOUS_VIEW_OFFSET = VIEW_OFFSET;

    PREVIOUS_PLAYER_CUBE = PLAYER_CUBE;
    PREVIOUS_PLAYER_SPEED = PLAYER_SPEED;

    memcpy(PREVIOUS_ENEMIES, ENEMIES, sizeof(Enemy) * LEN_ENEMIES);
    PREVIOUS_PLAYER_IN_VIEW = PLAYER_IN_VIEW;

    step(window);
    {
        const f32 blend = ((f32)delta) / ((f32)STEP_DURATION);
        EXIT_IF(blend < 0.0f);
        EXIT_IF(1.0f <= blend);

        VIEW_OFFSET =
            math_lerp_vec3f(PREVIOUS_VIEW_OFFSET, VIEW_OFFSET, blend);

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
    u64 frame_prev = time_nanoseconds();
    u64 frame_count = 0;

    printf("\n\n");
    while (!glfwWindowShouldClose(window)) {
        const u64 now = time_nanoseconds();

        // NOTE: See `http://www.opengl-tutorial.org/miscellaneous/an-fps-counter/`.
        if (NANO_PER_SECOND <= (now - frame_prev)) {
            printf("\033[2A"
                   "%7.4f ms/f\n"
                   "%7lu f/s\n",
                   (NANO_PER_SECOND / (f64)frame_count) / NANO_PER_MILLI,
                   frame_count);
            frame_prev += NANO_PER_SECOND;
            frame_count = 0;
        }

        update(window);
        graphics_draw(window);

        const u64 elapsed = time_nanoseconds() - now;
        if (elapsed < FRAME_DURATION) {
            const u64 remaining = FRAME_DURATION - elapsed;
            EXIT_IF(usleep((u32)(remaining / NANO_PER_MICRO)));
        }

        ++frame_count;
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

    pcg_rng_seed(time_nanoseconds(), 1);
    init();

    loop(window);

    graphics_free();
    glfwTerminate();
    return OK;
}
