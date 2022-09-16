#include "config.h"

#define GL_GLEXT_PROTOTYPES

#include <GLFW/glfw3.h>

static Vec3f VIEW_OFFSET = {0};

static Vec3f PLAYER_SPEED = {0};

static Box BOXES[LEN_CUBES];

#define CAP_LINES 2
static Line LINES[CAP_LINES];

#define CAP_VAO 2
static u32 VAO[CAP_VAO];

#define CAP_VBO 2
static u32 VBO[CAP_VBO];

#define CAP_EBO 1
static u32 EBO[CAP_EBO];

#define CAP_INSTANCE_VBO 2
static u32 INSTANCE_VBO[CAP_INSTANCE_VBO];

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
    EXIT_IF_GL_ERROR();
    return program;
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
    {
        Vec3f speed = PLAYER_SPEED;
        Vec3f remaining = PLAYER_SPEED;
        u8    hit = 0;
        for (u32 _ = 0; _ < 3; ++_) {
            set_box_from_cube(&PLAYER, &BOXES[0]);
            Collision collision = {0};
            for (u32 i = 1; i < LEN_CUBES; ++i) {
                const Collision candidate =
                    get_box_collision(&BOXES[0], &BOXES[i], &speed);
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
            if (!collision.hit) {
                PLAYER.translate.x += speed.x;
                PLAYER.translate.y += speed.y;
                PLAYER.translate.z += speed.z;
                break;
            }
            speed.x *= collision.time;
            speed.y *= collision.time;
            speed.z *= collision.time;
            PLAYER.translate.x += speed.x;
            PLAYER.translate.y += speed.y;
            PLAYER.translate.z += speed.z;
            remaining.x -= speed.x;
            remaining.y -= speed.y;
            remaining.z -= speed.z;
            switch (collision.hit) {
            case HIT_X: {
                remaining.x = 0.0f;
                break;
            }
            case HIT_Y: {
                remaining.y = 0.0f;
                break;
            }
            case HIT_Z: {
                remaining.z = 0.0f;
                break;
            }
            case HIT_NONE:
            default: {
                EXIT();
            }
            }
            speed.x = remaining.x;
            speed.y = remaining.y;
            speed.z = remaining.z;
            hit |= collision.hit;
        }
        if (hit & HIT_X) {
            PLAYER_SPEED.x = 0.0f;
        }
        if (hit & HIT_Y) {
            PLAYER_SPEED.y = 0.0f;
        }
        if (hit & HIT_Z) {
            PLAYER_SPEED.z = 0.0f;
        }
    }
    if (PLAYER.translate.y < FLOOR) {
        PLAYER.translate = PLAYER_TRANSLATE_INIT;
        PLAYER_SPEED = (Vec3f){0};
    }
    VIEW_OFFSET.x -= (VIEW_OFFSET.x - PLAYER.translate.x) * CAMERA_LATENCY;
    VIEW_OFFSET.z -= (VIEW_OFFSET.z - PLAYER.translate.z) * CAMERA_LATENCY;
}

#define BIND_BUFFER(object, data, size, target, usage) \
    do {                                               \
        glBindBuffer(target, object);                  \
        glBufferData(target, size, data, usage);       \
        EXIT_IF_GL_ERROR();                            \
    } while (FALSE)

#define SET_VERTEX_ATTRIB(index, size, stride, offset) \
    do {                                               \
        glEnableVertexAttribArray(index);              \
        glVertexAttribPointer(index,                   \
                              size,                    \
                              GL_FLOAT,                \
                              FALSE,                   \
                              stride,                  \
                              (void*)(offset));        \
        EXIT_IF_GL_ERROR();                            \
    } while (FALSE)

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

    const u32 cube_program = get_program(PATH_CUBE_VERT, PATH_CUBE_FRAG);
    const u32 line_program = get_program(PATH_LINE_VERT, PATH_LINE_FRAG);

    glGenVertexArrays(CAP_VAO, &VAO[0]);
    glGenBuffers(CAP_VBO, &VBO[0]);
    glGenBuffers(CAP_EBO, &EBO[0]);
    glGenBuffers(CAP_INSTANCE_VBO, &INSTANCE_VBO[0]);
    EXIT_IF_GL_ERROR();

    glUseProgram(cube_program);
    glBindVertexArray(VAO[0]);

    BIND_BUFFER(VBO[0],
                CUBE_VERTICES,
                sizeof(CUBE_VERTICES),
                GL_ARRAY_BUFFER,
                GL_STATIC_DRAW);

#define SIZE   (sizeof(Vec3f) / sizeof(f32))
#define STRIDE sizeof(CUBE_VERTICES[0])
    {
        const u32 index =
            (u32)glGetAttribLocation(cube_program, "VERT_IN_POSITION");
        SET_VERTEX_ATTRIB(index, SIZE, STRIDE, offsetof(Vertex, position));
    }
    {
        const u32 index =
            (u32)glGetAttribLocation(cube_program, "VERT_IN_NORMAL");
        SET_VERTEX_ATTRIB(index, SIZE, STRIDE, offsetof(Vertex, normal));
    }
#undef SIZE
#undef STRIDE

    BIND_BUFFER(EBO[0],
                CUBE_INDICES,
                sizeof(CUBE_INDICES),
                GL_ELEMENT_ARRAY_BUFFER,
                GL_STATIC_DRAW);

    BIND_BUFFER(INSTANCE_VBO[0],
                CUBES,
                sizeof(CUBES),
                GL_ARRAY_BUFFER,
                GL_DYNAMIC_DRAW);
#define SIZE   (sizeof(Vec3f) / sizeof(f32))
#define STRIDE sizeof(CUBES[0])
    {
        const u32 index =
            (u32)glGetAttribLocation(cube_program, "VERT_IN_TRANSLATE");
        SET_VERTEX_ATTRIB(index, SIZE, STRIDE, offsetof(Cube, translate));
        glVertexAttribDivisor(index, 1);
        EXIT_IF_GL_ERROR();
    }
    {
        const u32 index =
            (u32)glGetAttribLocation(cube_program, "VERT_IN_SCALE");
        SET_VERTEX_ATTRIB(index, SIZE, STRIDE, offsetof(Cube, scale));
        glVertexAttribDivisor(index, 1);
        EXIT_IF_GL_ERROR();
    }
#undef SIZE
#define SIZE (sizeof(Vec4f) / sizeof(f32))
    {
        const u32 index =
            (u32)glGetAttribLocation(cube_program, "VERT_IN_COLOR");
        SET_VERTEX_ATTRIB(index, SIZE, STRIDE, offsetof(Cube, color));
        glVertexAttribDivisor(index, 1);
        EXIT_IF_GL_ERROR();
    }
#undef SIZE
#undef STRIDE

    const Mat4 projection =
        perspective(FOV_DEGREES, ASPECT_RATIO, VIEW_NEAR, VIEW_FAR);
    glUniformMatrix4fv(glGetUniformLocation(cube_program, "PROJECTION"),
                       1,
                       FALSE,
                       &projection.column_row[0][0]);
    glUniform3f(glGetUniformLocation(cube_program, "VIEW_FROM"),
                VIEW_FROM.x,
                VIEW_FROM.y,
                VIEW_FROM.z);
    const i32 cube_uniform_view = glGetUniformLocation(cube_program, "VIEW");
    EXIT_IF_GL_ERROR();

    const u64 cube_vbo_index =
        (u64)glGetAttribLocation(cube_program, "VERT_IN_POSITION");

    glUseProgram(line_program);
    glBindVertexArray(VAO[1]);

    BIND_BUFFER(VBO[1],
                LINE_VERTICES,
                sizeof(LINE_VERTICES),
                GL_ARRAY_BUFFER,
                GL_STATIC_DRAW);

#define SIZE   (sizeof(Vec3f) / sizeof(f32))
#define STRIDE sizeof(Vec3f)
    {
        const u32 index =
            (u32)glGetAttribLocation(line_program, "VERT_IN_POSITION");
        SET_VERTEX_ATTRIB(index, SIZE, STRIDE, offsetof(Vec3f, x));
    }
#undef SIZE
#undef STRIDE

    BIND_BUFFER(INSTANCE_VBO[1],
                LINES,
                sizeof(LINES),
                GL_ARRAY_BUFFER,
                GL_DYNAMIC_DRAW);
#define SIZE   (sizeof(Vec3f) / sizeof(f32))
#define STRIDE sizeof(LINES[0])
    {
        const u32 index =
            (u32)glGetAttribLocation(line_program, "VERT_IN_TRANSLATE");
        SET_VERTEX_ATTRIB(index, SIZE, STRIDE, offsetof(Line, translate));
        glVertexAttribDivisor(index, 1);
        EXIT_IF_GL_ERROR();
    }
    {
        const u32 index =
            (u32)glGetAttribLocation(line_program, "VERT_IN_SCALE");
        SET_VERTEX_ATTRIB(index, SIZE, STRIDE, offsetof(Line, scale));
        glVertexAttribDivisor(index, 1);
        EXIT_IF_GL_ERROR();
    }
#undef SIZE
#define SIZE (sizeof(Vec4f) / sizeof(f32))
    {
        const u32 index =
            (u32)glGetAttribLocation(line_program, "VERT_IN_COLOR");
        SET_VERTEX_ATTRIB(index, SIZE, STRIDE, offsetof(Line, color));
        glVertexAttribDivisor(index, 1);
        EXIT_IF_GL_ERROR();
    }
#undef SIZE
#undef STRIDE

    glUniformMatrix4fv(glGetUniformLocation(line_program, "PROJECTION"),
                       1,
                       FALSE,
                       &projection.column_row[0][0]);
    const i32 line_uniform_view = glGetUniformLocation(line_program, "VIEW");

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

                set_line_between(&PLAYER, &CUBES[1], &LINES[0]);
                set_line_between(&PLAYER, &CUBES[2], &LINES[1]);

                glUseProgram(cube_program);
                glBindVertexArray(VAO[0]);
                glUniformMatrix4fv(cube_uniform_view,
                                   1,
                                   FALSE,
                                   &view.column_row[0][0]);

                glBindBuffer(GL_ARRAY_BUFFER, INSTANCE_VBO[0]);
                glBufferSubData(GL_ARRAY_BUFFER,
                                0,
                                sizeof(PLAYER.translate),
                                &PLAYER.translate);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                glDrawElementsInstanced(GL_TRIANGLES,
                                        sizeof(CUBE_INDICES) / (sizeof(u8)),
                                        GL_UNSIGNED_BYTE,
                                        (void*)cube_vbo_index,
                                        LEN_CUBES);

                glUseProgram(line_program);
                glUniformMatrix4fv(line_uniform_view,
                                   1,
                                   FALSE,
                                   &view.column_row[0][0]);
                glBindVertexArray(VAO[1]);
                glBindBuffer(GL_ARRAY_BUFFER, INSTANCE_VBO[1]);
                glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(LINES), &LINES[0]);
                glDrawArraysInstanced(GL_LINES, 0, 2, CAP_LINES);

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

    glDeleteProgram(cube_program);
    glDeleteProgram(line_program);
    glDeleteVertexArrays(CAP_VAO, &VAO[0]);
    glDeleteBuffers(CAP_VBO, &VBO[0]);
    glDeleteBuffers(CAP_EBO, &EBO[0]);
    glDeleteBuffers(CAP_INSTANCE_VBO, &INSTANCE_VBO[0]);
    glfwTerminate();
    return OK;
}
