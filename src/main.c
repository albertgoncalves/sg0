#include "prelude.h"

#include <math.h>

#define GL_GLEXT_PROTOTYPES

#include <GLFW/glfw3.h>

#define WINDOW_WIDTH  1024
#define WINDOW_HEIGHT 768
#define WINDOW_NAME   __FILE__

#define BACKGROUND_COLOR 0.20f, 0.20f, 0.20f, 1.0f

#define FOV_DEGREES 45.0f

#define VIEW_NEAR 0.1f
#define VIEW_FAR  100.0f
#define VIEW_UP   ((Vec3f){0.0f, 1.0f, 0.0f})

#define VIEW_FROM ((Vec3f){0.0f, 17.5f, 10.0f})
#define VIEW_TO   ((Vec3f){0.0f, 0.0f, 0.0f})

static Vec3f VIEW_OFFSET = {0};

static u32 INDEX_VERTEX;

#define RUN      0.001875f
#define FRICTION 0.9675f

#define CAMERA_LATENCY (1.0f / 200.0f)

typedef struct {
    Vec3f translate;
    Vec3f scale;
    Vec3f color;
} Rect;

static Rect RECTS[] = {
    {{0}, {1.0f, 1.0f, 1.0f}, {1.0f, 0.0f, 0.0f}},
    {{0.0f, -1.0f, 0.0f}, {20.f, 1.0f, 20.f}, {0.0f, 0.25f, 0.25f}},
    {{-2.0f, 0.0f, 0.0f}, {0.5f, 5.0f, 5.0f}, {0.25f, 0.0f, 0.25f}},
};

#define LEN_RECTS (sizeof(RECTS) / sizeof(RECTS[0]))

#define PLAYER_POSITION (RECTS[0].translate)

static Vec3f PLAYER_SPEED = {0};

#define FRAME_UPDATE_COUNT 8
#define FRAME_DURATION     (NANO_PER_SECOND / (60 + 1))
#define FRAME_UPDATE_STEP  (FRAME_DURATION / FRAME_UPDATE_COUNT)

typedef struct {
    Vec3f position;
    Vec3f normal;
} Vertex;

static const Vertex VERTICES[] = {
    {{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}},
    {{0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}},
    {{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}},
    {{-0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}},
    {{-0.5f, -0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
    {{0.5f, -0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
    {{0.5f, 0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
    {{-0.5f, 0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
    {{-0.5f, 0.5f, 0.5f}, {-1.0f, 0.0f, 0.0f}},
    {{-0.5f, 0.5f, -0.5f}, {-1.0f, 0.0f, 0.0f}},
    {{-0.5f, -0.5f, -0.5f}, {-1.0f, 0.0f, 0.0f}},
    {{-0.5f, -0.5f, 0.5f}, {-1.0f, 0.0f, 0.0f}},
    {{0.5f, 0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}},
    {{0.5f, 0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
    {{0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
    {{0.5f, -0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}},
    {{-0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f}},
    {{0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f}},
    {{0.5f, -0.5f, 0.5f}, {0.0f, -1.0f, 0.0f}},
    {{-0.5f, -0.5f, 0.5f}, {0.0f, -1.0f, 0.0f}},
    {{-0.5f, 0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
    {{0.5f, 0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
    {{0.5f, 0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
    {{-0.5f, 0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
};

static const Vec3u INDICES[] = {
    {0, 1, 2},
    {2, 3, 0},
    {4, 5, 6},
    {6, 7, 4},
    {8, 9, 10},
    {10, 11, 8},
    {12, 13, 14},
    {14, 15, 12},
    {16, 17, 18},
    {18, 19, 16},
    {20, 21, 22},
    {22, 23, 20},
};

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

static u32 get_vbo(u32 program) {
    u32 vbo;
    BIND_BUFFER(vbo, VERTICES, GL_ARRAY_BUFFER, GL_STATIC_DRAW);
#define SIZE   (sizeof(Vec3f) / sizeof(f32))
#define STRIDE sizeof(VERTICES[0])
    {
        INDEX_VERTEX = (u32)glGetAttribLocation(program, "VERT_IN_POSITION");
        glEnableVertexAttribArray(INDEX_VERTEX);
        glVertexAttribPointer(INDEX_VERTEX,
                              SIZE,
                              GL_FLOAT,
                              FALSE,
                              STRIDE,
                              (void*)offsetof(Vertex, position));
        EXIT_IF_GL_ERROR();
    }
    {
        u32 index = (u32)glGetAttribLocation(program, "VERT_IN_NORMAL");
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
    return vbo;
}

static u32 get_instance_vbo(u32 program) {
    u32 instance_vbo;
    BIND_BUFFER(instance_vbo, RECTS, GL_ARRAY_BUFFER, GL_DYNAMIC_DRAW);
#define SIZE   (sizeof(Vec3f) / sizeof(f32))
#define STRIDE sizeof(RECTS[0])
    {
        u32 index = (u32)glGetAttribLocation(program, "VERT_IN_TRANSLATE");
        glEnableVertexAttribArray(index);
        glVertexAttribPointer(index,
                              SIZE,
                              GL_FLOAT,
                              FALSE,
                              STRIDE,
                              (void*)offsetof(Rect, translate));
        glVertexAttribDivisor(index, 1);
        EXIT_IF_GL_ERROR();
    }
    {
        u32 index = (u32)glGetAttribLocation(program, "VERT_IN_SCALE");
        glEnableVertexAttribArray(index);
        glVertexAttribPointer(index,
                              SIZE,
                              GL_FLOAT,
                              FALSE,
                              STRIDE,
                              (void*)offsetof(Rect, scale));
        glVertexAttribDivisor(index, 1);
        EXIT_IF_GL_ERROR();
    }
    {
        u32 index = (u32)glGetAttribLocation(program, "VERT_IN_COLOR");
        glEnableVertexAttribArray(index);
        glVertexAttribPointer(index,
                              SIZE,
                              GL_FLOAT,
                              FALSE,
                              STRIDE,
                              (void*)offsetof(Rect, color));
        glVertexAttribDivisor(index, 1);
        EXIT_IF_GL_ERROR();
    }
#undef SIZE
#undef STRIDE
    return instance_vbo;
}

static u32 get_ebo(void) {
    u32 ebo;
    BIND_BUFFER(ebo, INDICES, GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW);
    return ebo;
}

static void update(GLFWwindow* window) {
    glfwPollEvents();
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
        f32 x = move.x * move.x;
        f32 z = move.z * move.z;
        f32 l = sqrtf(x + z);
        PLAYER_SPEED.x += (move.x / l) * RUN;
        PLAYER_SPEED.z += (move.z / l) * RUN;
    }
    PLAYER_SPEED.x *= FRICTION;
    PLAYER_SPEED.z *= FRICTION;
    PLAYER_POSITION.x += PLAYER_SPEED.x;
    PLAYER_POSITION.z += PLAYER_SPEED.z;
    VIEW_OFFSET.x -= (VIEW_OFFSET.x - PLAYER_POSITION.x) * CAMERA_LATENCY;
    VIEW_OFFSET.z -= (VIEW_OFFSET.z - PLAYER_POSITION.z) * CAMERA_LATENCY;
}

static void render(GLFWwindow* window, u32 program) {
    glUniform3f(glGetUniformLocation(program, "VIEW_OFFSET"),
                VIEW_OFFSET.x,
                VIEW_OFFSET.y,
                VIEW_OFFSET.z);
    glBufferSubData(GL_ARRAY_BUFFER,
                    0,
                    sizeof(PLAYER_POSITION),
                    &PLAYER_POSITION);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDrawElementsInstanced(GL_TRIANGLES,
                            sizeof(INDICES) / (sizeof(u8)),
                            GL_UNSIGNED_BYTE,
                            (void*)((u64)INDEX_VERTEX),
                            (i32)LEN_RECTS);
    EXIT_IF_GL_ERROR();
    glfwSwapBuffers(window);
}

i32 main(i32 n, const char** args) {
    EXIT_IF(n < 2);
    printf("GLFW version : %s\n", glfwGetVersionString());

    EXIT_IF(!glfwInit());
    glfwSetErrorCallback(callback_error);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, FALSE);
    GLFWwindow* window =
        glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_NAME, NULL, NULL);
    EXIT_IF(!window);

    glfwSetKeyCallback(window, callback_key);
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    glClearColor(BACKGROUND_COLOR);
    glEnable(GL_DEPTH_TEST);
    EXIT_IF_GL_ERROR();

    printf("GL_VENDOR    : %s\n"
           "GL_RENDERER  : %s\n",
           glGetString(GL_VENDOR),
           glGetString(GL_RENDERER));

    const u32 program = get_program(args[1], args[2]);
    const u32 vao = get_vao();
    const u32 vbo = get_vbo(program);
    const u32 ebo = get_ebo();
    const u32 instance_vbo = get_instance_vbo(program);

    glUniform1f(glGetUniformLocation(program, "FOV_DEGREES"), FOV_DEGREES);
    glUniform1f(glGetUniformLocation(program, "ASPECT_RATIO"),
                ((f32)WINDOW_WIDTH) / ((f32)WINDOW_HEIGHT));
    glUniform1f(glGetUniformLocation(program, "VIEW_NEAR"), VIEW_NEAR);
    glUniform1f(glGetUniformLocation(program, "VIEW_FAR"), VIEW_FAR);
    glUniform3f(glGetUniformLocation(program, "VIEW_UP"),
                VIEW_UP.x,
                VIEW_UP.y,
                VIEW_UP.z);
    glUniform3f(glGetUniformLocation(program, "VIEW_FROM"),
                VIEW_FROM.x,
                VIEW_FROM.y,
                VIEW_FROM.z);
    glUniform3f(glGetUniformLocation(program, "VIEW_TO"),
                VIEW_TO.x,
                VIEW_TO.y,
                VIEW_TO.z);
    EXIT_IF_GL_ERROR();

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
            render(window, program);
            const u64 elapsed = now_ns() - now;
            if (elapsed < FRAME_DURATION) {
                EXIT_IF(usleep(
                    (u32)((FRAME_DURATION - elapsed) / NANO_PER_MICRO)));
            }
        }
    }

    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);
    glDeleteBuffers(1, &instance_vbo);
    glDeleteProgram(program);
    glfwTerminate();
    return OK;
}
