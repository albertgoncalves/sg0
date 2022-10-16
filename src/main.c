#include "enemy.h"
#include "graphics.h"
#include "player.h"
#include "time.h"

static Vec3f get_move(GLFWwindow* window) {
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
    return normalize(move);
}

static void update(GLFWwindow* window,
                   u64         now,
                   u64*        update_time,
                   u64*        update_delta,
                   Uniforms*   uniforms) {
    for (*update_delta += now - *update_time;
         FRAME_UPDATE_STEP < *update_delta;
         *update_delta -= FRAME_UPDATE_STEP)
    {
        glfwPollEvents();
        update_player(get_move(window));
        update_sprites();
        animate_player();
        update_camera(PLAYER_CUBE.translate);
    }
    *update_time = now;
    uniforms->view = get_view();
}

static void loop(GLFWwindow* window,
                 Uniforms*   uniforms,
                 u32         cube_program,
                 u32         line_program,
                 u32         sprite_program) {
    u64 update_time = now_ns();
    u64 update_delta = 0;
    u64 frame_time = update_time;
    u64 frame_count = 0;
    printf("\n\n");
    while (!glfwWindowShouldClose(window)) {
        const u64 now = now_ns();
        {
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
        }
        update(window, now, &update_time, &update_delta, uniforms);
        draw(window, uniforms, cube_program, line_program, sprite_program);
        wait(now);
    }
}

i32 main(void) {
    printf("\n"
           "sizeof(BUFFER)  : %zu\n"
           "sizeof(CUBES)   : %zu\n"
           "sizeof(BOXES)   : %zu\n"
           "sizeof(LINES)   : %zu\n"
           "sizeof(SPRITES) : %zu\n"
           "\n",
           sizeof(BUFFER),
           sizeof(CUBES),
           sizeof(BOXES),
           sizeof(LINES),
           sizeof(SPRITES));
    printf("GLFW version : %s\n", glfwGetVersionString());
    GLFWwindow* window = init_window();
    printf("GL_VENDOR    : %s\n"
           "GL_RENDERER  : %s\n"
           "\n",
           glGetString(GL_VENDOR),
           glGetString(GL_RENDERER));

    init_graphics();
    Uniforms  uniforms = init_uniforms();
    const u32 cube_program = init_cubes();
    const u32 line_program = init_lines();
    const u32 sprite_program = init_sprites();

    init_player();
    init_enemies();
    init_world();
    loop(window, &uniforms, cube_program, line_program, sprite_program);

    free_graphics();
    glDeleteProgram(cube_program);
    glDeleteProgram(line_program);
    glDeleteProgram(sprite_program);

    glfwTerminate();
    return OK;
}
