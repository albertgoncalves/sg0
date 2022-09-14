#include "config.h"

#define GL_GLEXT_PROTOTYPES

#include <GLFW/glfw3.h>

static Vec3f VIEW_OFFSET = {0};

static Vec3f PLAYER_SPEED = {0};

static Box BOXES[LEN_CUBES];

#define EXIT_IF_GL_ERROR()                                 \
    do {                                                   \
        switch (glGetError()) {                            \
        case GL_INVALID_ENUM: {                            \
            EXIT_WITH("GL_INVALID_ENUM");                  \
        }                                                  \
        case GL_INVALID_VALUE: {                           \
            EXIT_WITH("GL_INVALID_VALUE");                 \
        }                                                  \
        case GL_INVALID_OPERATION: {                       \
            EXIT_WITH("GL_INVALID_OPERATION");             \
        }                                                  \
        case GL_INVALID_FRAMEBUFFER_OPERATION: {           \
            EXIT_WITH("GL_INVALID_FRAMEBUFFER_OPERATION"); \
        }                                                  \
        case GL_OUT_OF_MEMORY: {                           \
            EXIT_WITH("GL_OUT_OF_MEMORY");                 \
        }                                                  \
        case GL_NO_ERROR: {                                \
            break;                                         \
        }                                                  \
        }                                                  \
    } while (FALSE)

ATTRIBUTE(noreturn) static void callback_error(i32 code, const char* error) {
    printf("%d: %s\n", code, error);
    _exit(ERROR);
}

static void callback_key(GLFWwindow* window, i32 key, i32, i32 action, i32) {
    if (action != GLFW_PRESS) {
        return;
    }
    switch (key) {
    case GLFW_KEY_ESCAPE: {
        glfwSetWindowShouldClose(window, TRUE);
        break;
    }
    }
}

static void compile_shader(const char* path, u32 shader) {
    const MemMap map = path_to_map(path);
    const char*  source = map_to_buffer(map);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);
    {
        i32 status = 0;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
        if (!status) {
            glGetShaderInfoLog(shader,
                               (i32)(CAP_BUFFER - LEN_BUFFER),
                               NULL,
                               &BUFFER[LEN_BUFFER]);
            printf("%s", &BUFFER[LEN_BUFFER]);
            EXIT();
        }
    }
    EXIT_IF(munmap(map.address, map.len));
}

static u32 get_program(const char* source_vert, const char* source_frag) {
    const u32 program = glCreateProgram();
    const u32 shader_vert = glCreateShader(GL_VERTEX_SHADER);
    const u32 shader_frag = glCreateShader(GL_FRAGMENT_SHADER);
    compile_shader(source_vert, shader_vert);
    compile_shader(source_frag, shader_frag);
    glAttachShader(program, shader_vert);
    glAttachShader(program, shader_frag);
    glLinkProgram(program);
    {
        i32 status = 0;
        glGetProgramiv(program, GL_LINK_STATUS, &status);
        if (!status) {
            glGetProgramInfoLog(program,
                                (i32)(CAP_BUFFER - LEN_BUFFER),
                                NULL,
                                &BUFFER[LEN_BUFFER]);
            printf("%s", &BUFFER[LEN_BUFFER]);
            EXIT();
        }
    }
    glDeleteShader(shader_vert);
    glDeleteShader(shader_frag);
    glUseProgram(program);
    EXIT_IF_GL_ERROR();
    return program;
}

static u32 get_vao(void) {
    u32 vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    EXIT_IF_GL_ERROR();
    return vao;
}

#define BIND_BUFFER(object, array, target, usage)          \
    do {                                                   \
        glGenBuffers(1, &object);                          \
        glBindBuffer(target, object);                      \
        glBufferData(target, sizeof(array), array, usage); \
        EXIT_IF_GL_ERROR();                                \
    } while (FALSE)

static u32 get_vbo(void) {
    u32 vbo;
    BIND_BUFFER(vbo, VERTICES, GL_ARRAY_BUFFER, GL_STATIC_DRAW);
    return vbo;
}

static u32 get_vbo_index(u32 program) {
#define SIZE   (sizeof(Vec3f) / sizeof(f32))
#define STRIDE sizeof(VERTICES[0])
    u32 vbo_index;
    {
        vbo_index = (u32)glGetAttribLocation(program, "VERT_IN_POSITION");
        glEnableVertexAttribArray(vbo_index);
        glVertexAttribPointer(vbo_index,
                              SIZE,
                              GL_FLOAT,
                              FALSE,
                              STRIDE,
                              (void*)offsetof(Vertex, position));
        EXIT_IF_GL_ERROR();
    }
    {
        const u32 index = (u32)glGetAttribLocation(program, "VERT_IN_NORMAL");
        glEnableVertexAttribArray(index);
        glVertexAttribPointer(index,
                              SIZE,
                              GL_FLOAT,
                              FALSE,
                              STRIDE,
                              (void*)offsetof(Vertex, normal));
        EXIT_IF_GL_ERROR();
    }
#undef SIZE
#undef STRIDE
    return vbo_index;
}

static u32 get_instance_vbo(u32 program) {
    u32 instance_vbo;
    BIND_BUFFER(instance_vbo, CUBES, GL_ARRAY_BUFFER, GL_DYNAMIC_DRAW);
#define SIZE   (sizeof(Vec3f) / sizeof(f32))
#define STRIDE sizeof(CUBES[0])
    {
        const u32 index =
            (u32)glGetAttribLocation(program, "VERT_IN_TRANSLATE");
        glEnableVertexAttribArray(index);
        glVertexAttribPointer(index,
                              SIZE,
                              GL_FLOAT,
                              FALSE,
                              STRIDE,
                              (void*)offsetof(Cube, translate));
        glVertexAttribDivisor(index, 1);
        EXIT_IF_GL_ERROR();
    }
    {
        const u32 index = (u32)glGetAttribLocation(program, "VERT_IN_SCALE");
        glEnableVertexAttribArray(index);
        glVertexAttribPointer(index,
                              SIZE,
                              GL_FLOAT,
                              FALSE,
                              STRIDE,
                              (void*)offsetof(Cube, scale));
        glVertexAttribDivisor(index, 1);
        EXIT_IF_GL_ERROR();
    }
#undef SIZE
    {
        const u32 index = (u32)glGetAttribLocation(program, "VERT_IN_COLOR");
        glEnableVertexAttribArray(index);
        glVertexAttribPointer(index,
                              sizeof(Vec4f) / sizeof(f32),
                              GL_FLOAT,
                              FALSE,
                              STRIDE,
                              (void*)offsetof(Cube, color));
        glVertexAttribDivisor(index, 1);
        EXIT_IF_GL_ERROR();
    }
#undef STRIDE
    return instance_vbo;
}

static u32 get_ebo(void) {
    u32 ebo;
    BIND_BUFFER(ebo, INDICES, GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW);
    return ebo;
}

static Bool resolve_collisions(void) {
    Collision collision = {0};
    for (u32 i = 1; i < LEN_CUBES; ++i) {
        const Collision candidate =
            get_box_collision(&BOXES[0], &BOXES[i], &PLAYER_SPEED);
        if (!candidate.hit) {
            continue;
        }
        if (!collision.hit || (candidate.time < collision.time) ||
            ((candidate.time == collision.time) &&
             (collision.overlap < candidate.overlap)))
        {
            collision = candidate;
        }
    }
    switch (collision.hit) {
    case HIT_NONE: {
        return TRUE;
    }
    case HIT_X: {
        PLAYER_SPEED.x *= collision.time;
        break;
    }
    case HIT_Y: {
        PLAYER_SPEED.y *= collision.time;
        break;
    }
    case HIT_Z: {
        PLAYER_SPEED.z *= collision.time;
        break;
    }
    default: {
        EXIT();
    }
    }
    return FALSE;
}

static void update(GLFWwindow* window) {
    glfwPollEvents();
    EXIT_IF(0.0f < PLAYER_SPEED.y);
    if (PLAYER_SPEED.y < 0.0f) {
        PLAYER_SPEED.x *= DRAG;
        PLAYER_SPEED.z *= DRAG;
    } else {
        Vec3f move = {0};
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            move.z -= 1.0f;
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            move.z += 1.0f;
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            move.x += 1.0f;
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            move.x -= 1.0f;
        }
        if ((move.x != 0.0f) || (move.z != 0.0f)) {
            move = normalize(move);
            PLAYER_SPEED.x += move.x * RUN;
            PLAYER_SPEED.z += move.z * RUN;
        }
        PLAYER_SPEED.x *= FRICTION;
        PLAYER_SPEED.z *= FRICTION;
    }
    PLAYER_SPEED.y -= GRAVITY;
    set_box_from_cube(&CUBES[0], &BOXES[0]);
    for (u32 _ = 0; _ < 3; ++_) {
        if (resolve_collisions()) {
            break;
        }
    }
    PLAYER.translate.x += PLAYER_SPEED.x;
    PLAYER.translate.y += PLAYER_SPEED.y;
    PLAYER.translate.z += PLAYER_SPEED.z;
    if (PLAYER.translate.y < FLOOR) {
        PLAYER.translate = PLAYER_TRANSLATE_INIT;
        PLAYER_SPEED = (Vec3f){0};
    }
    VIEW_OFFSET.x -= (VIEW_OFFSET.x - PLAYER.translate.x) * CAMERA_LATENCY;
    VIEW_OFFSET.z -= (VIEW_OFFSET.z - PLAYER.translate.z) * CAMERA_LATENCY;
}

i32 main(void) {
    printf("GLFW version : %s\n", glfwGetVersionString());

    EXIT_IF(!glfwInit());
    glfwSetErrorCallback(callback_error);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, FALSE);
    GLFWwindow* window =
        glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, __FILE__, NULL, NULL);
    EXIT_IF(!window);

    glfwSetKeyCallback(window, callback_key);
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    glClearColor(BACKGROUND_COLOR);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    EXIT_IF_GL_ERROR();

    printf("GL_VENDOR    : %s\n"
           "GL_RENDERER  : %s\n",
           glGetString(GL_VENDOR),
           glGetString(GL_RENDERER));

    const u32 display_program =
        get_program(PATH_DISPLAY_VERT, PATH_DISPLAY_FRAG);
    const u32 vao = get_vao();
    const u32 vbo = get_vbo();
    const u32 vbo_index = get_vbo_index(display_program);
    const u32 ebo = get_ebo();
    const u32 instance_vbo = get_instance_vbo(display_program);

    {
        const Mat4 projection =
            perspective(FOV_DEGREES, ASPECT_RATIO, VIEW_NEAR, VIEW_FAR);
        glUniformMatrix4fv(glGetUniformLocation(display_program, "PROJECTION"),
                           1,
                           FALSE,
                           &projection.column_row[0][0]);
    }
    glUniform3f(glGetUniformLocation(display_program, "VIEW_FROM"),
                VIEW_FROM.x,
                VIEW_FROM.y,
                VIEW_FROM.z);
    const i32 uniform_view = glGetUniformLocation(display_program, "VIEW");
    EXIT_IF_GL_ERROR();

    for (u32 i = 0; i < LEN_CUBES; ++i) {
        set_box_from_cube(&CUBES[i], &BOXES[i]);
    }
    {
        u64 update_time = now_ns();
        u64 update_delta = 0;
        u64 frame_time = now_ns();
        u64 frame_count = 0;
        printf("\n\n");
        while (!glfwWindowShouldClose(window)) {
            const u64 now = now_ns();
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
            for (update_delta += now - update_time;
                 FRAME_UPDATE_STEP < update_delta;
                 update_delta -= FRAME_UPDATE_STEP)
            {
                update(window);
            }
            update_time = now;
            {
                const Vec3f view_from = (Vec3f){
                    .x = VIEW_FROM.x + VIEW_OFFSET.x,
                    .y = VIEW_FROM.y + VIEW_OFFSET.y,
                    .z = VIEW_FROM.z + VIEW_OFFSET.z,
                };
                const Vec3f view_to = (Vec3f){
                    .x = VIEW_TO.x + VIEW_OFFSET.x,
                    .y = VIEW_TO.y + VIEW_OFFSET.y,
                    .z = VIEW_TO.z + VIEW_OFFSET.z,
                };
                const Mat4 view = look_at(view_from, view_to, VIEW_UP);
                glUniformMatrix4fv(uniform_view,
                                   1,
                                   FALSE,
                                   &view.column_row[0][0]);
                glBufferSubData(GL_ARRAY_BUFFER,
                                0,
                                sizeof(PLAYER.translate),
                                &PLAYER.translate);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                glDrawElementsInstanced(GL_TRIANGLES,
                                        sizeof(INDICES) / (sizeof(u8)),
                                        GL_UNSIGNED_BYTE,
                                        (void*)((u64)vbo_index),
                                        (i32)LEN_CUBES);
                EXIT_IF_GL_ERROR();
                glfwSwapBuffers(window);
            }
            {
                const u64 elapsed = now_ns() - now;
                if (elapsed < FRAME_DURATION) {
                    EXIT_IF(usleep(
                        (u32)((FRAME_DURATION - elapsed) / NANO_PER_MICRO)));
                }
            }
        }
    }

    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);
    glDeleteBuffers(1, &instance_vbo);
    glDeleteProgram(display_program);
    glfwTerminate();
    return OK;
}
