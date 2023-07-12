#include "enemy.h"
#include "graphics.h"
#include "pcg_rng.h"
#include "player.h"
#include "sprite.h"
#include "string.h"
#include "time.h"
#include "world.h"

char BUFFER[CAP_BUFFER];
u32  LEN_BUFFER = 0;

Geom CUBES[CAP_CUBES];
u32  LEN_CUBES = CAP_PLAYER + CAP_ENEMIES;

Geom LINES[CAP_LINES];
u32  LEN_LINES = 0;

Sprite SPRITES[CAP_SPRITES];
u32    LEN_SPRITES = CAP_PLAYER;

Box BOXES[CAP_CUBES];

u32 LEN_WORLD = 0;

u32 OFFSET_WAYPOINTS = 0;

u64 SPRITE_TIME = 0;

#define FRAME_UPDATE_COUNT 6
#define FRAME_DURATION     (NANO_PER_SECOND / (60 + 1))
#define FRAME_UPDATE_STEP  (FRAME_DURATION / FRAME_UPDATE_COUNT)

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

static void update(GLFWwindow* window,
                   u64         now,
                   u64*        update_time,
                   u64*        update_delta) {
    for (*update_delta += now - *update_time;
         FRAME_UPDATE_STEP < *update_delta;
         *update_delta -= FRAME_UPDATE_STEP)
    {
        glfwPollEvents();
        player_update(input(window));
        enemy_update();
        sprite_update();
        player_animate();
        graphics_update_camera(PLAYER_CUBE.translate);
    }
    *update_time = now;
    graphics_update_uniforms();
}

static void loop(GLFWwindow* window,
                 u32         cube_program,
                 u32         line_program,
                 u32         sprite_program) {
    u64 update_time = time_nanoseconds();
    u64 update_delta = 0;
    u64 frame_time = update_time;
    u64 frame_count = 0;

    printf("\n\n");
    while (!glfwWindowShouldClose(window)) {
        const u64 now = time_nanoseconds();

        ++frame_count;
        // NOTE: See `http://www.opengl-tutorial.org/miscellaneous/an-fps-counter/`.
        if (NANO_PER_SECOND <= (now - frame_time)) {
            printf("\033[2A"
                   "%7.4f ms/f\n"
                   "%7lu f/s\n",
                   (NANO_PER_SECOND / (f64)frame_count) / NANO_PER_MILLI,
                   frame_count);
            frame_time += NANO_PER_SECOND;
            frame_count = 0;
        }

        update(window, now, &update_time, &update_delta);
        graphics_draw(window, cube_program, line_program, sprite_program);

        const u64 elapsed = time_nanoseconds() - now;
        if (elapsed < FRAME_DURATION) {
            EXIT_IF(
                usleep((u32)((FRAME_DURATION - elapsed) / NANO_PER_MICRO)));
        }
    }
}

i32 main(void) {
    printf("GLFW version : %s\n", glfwGetVersionString());
    GLFWwindow* window = graphics_window();
    printf("GL_VENDOR    : %s\n"
           "GL_RENDERER  : %s\n"
           "\n",
           glGetString(GL_VENDOR),
           glGetString(GL_RENDERER));

    graphics_init();
    const u32 cube_program = graphics_cubes();
    const u32 line_program = graphics_lines();
    const u32 sprite_program = graphics_sprites();

    pcg_rng_seed(time_nanoseconds(), 1);
    world_init();
    player_init();
    enemy_init();

    loop(window, cube_program, line_program, sprite_program);

    graphics_free();
    glDeleteProgram(cube_program);
    glDeleteProgram(line_program);
    glDeleteProgram(sprite_program);

    glfwTerminate();
    return OK;
}
