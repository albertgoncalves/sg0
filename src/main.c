#include "config.h"

#include <math.h>

#define GL_GLEXT_PROTOTYPES

#include <GLFW/glfw3.h>

static Vec3f VIEW_OFFSET = {0};

static Vec3f PLAYER_SPEED = {0};

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

static const f32 BORDER_COLOR[] = {0};

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
    BIND_BUFFER(instance_vbo, RECTS, GL_ARRAY_BUFFER, GL_DYNAMIC_DRAW);
#define SIZE   (sizeof(Vec3f) / sizeof(f32))
#define STRIDE sizeof(RECTS[0])
    {
        const u32 index =
            (u32)glGetAttribLocation(program, "VERT_IN_TRANSLATE");
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
        const u32 index = (u32)glGetAttribLocation(program, "VERT_IN_SCALE");
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
#undef SIZE
    {
        const u32 index = (u32)glGetAttribLocation(program, "VERT_IN_COLOR");
        glEnableVertexAttribArray(index);
        glVertexAttribPointer(index,
                              sizeof(Vec4f) / sizeof(f32),
                              GL_FLOAT,
                              FALSE,
                              STRIDE,
                              (void*)offsetof(Rect, color));
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

static u32 get_depth_map_fbo(void) {
    u32 depth_map_fbo;
    glGenFramebuffers(1, &depth_map_fbo);
    return depth_map_fbo;
}

static u32 get_depth_map(u32 depth_map_fbo) {
    u32 depth_map;
    glGenTextures(1, &depth_map);
    glBindTexture(GL_TEXTURE_2D, depth_map);
    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_DEPTH_COMPONENT,
                 WINDOW_WIDTH,
                 WINDOW_HEIGHT,
                 0,
                 GL_DEPTH_COMPONENT,
                 GL_FLOAT,
                 NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, BORDER_COLOR);

    glBindFramebuffer(GL_FRAMEBUFFER, depth_map_fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER,
                           GL_DEPTH_ATTACHMENT,
                           GL_TEXTURE_2D,
                           depth_map,
                           0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return depth_map;
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
        f32 len = sqrtf((move.x * move.x) + (move.z * move.z));
        PLAYER_SPEED.x += (move.x / len) * RUN;
        PLAYER_SPEED.z += (move.z / len) * RUN;
    }
    PLAYER_SPEED.x *= FRICTION;
    PLAYER_SPEED.z *= FRICTION;
    PLAYER_POSITION.x += PLAYER_SPEED.x;
    PLAYER_POSITION.z += PLAYER_SPEED.z;
    VIEW_OFFSET.x -= (VIEW_OFFSET.x - PLAYER_POSITION.x) * CAMERA_LATENCY;
    VIEW_OFFSET.z -= (VIEW_OFFSET.z - PLAYER_POSITION.z) * CAMERA_LATENCY;
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

    glUniform1f(glGetUniformLocation(display_program, "FOV_DEGREES"),
                FOV_DEGREES);
    glUniform1f(glGetUniformLocation(display_program, "ASPECT_RATIO"),
                ((f32)WINDOW_WIDTH) / ((f32)WINDOW_HEIGHT));
    glUniform1f(glGetUniformLocation(display_program, "VIEW_NEAR"), VIEW_NEAR);
    glUniform1f(glGetUniformLocation(display_program, "VIEW_FAR"), VIEW_FAR);
    glUniform3f(glGetUniformLocation(display_program, "VIEW_UP"),
                VIEW_UP.x,
                VIEW_UP.y,
                VIEW_UP.z);
    glUniform3f(glGetUniformLocation(display_program, "VIEW_FROM"),
                VIEW_FROM.x,
                VIEW_FROM.y,
                VIEW_FROM.z);
    glUniform3f(glGetUniformLocation(display_program, "VIEW_TO"),
                VIEW_TO.x,
                VIEW_TO.y,
                VIEW_TO.z);
    glUniform3f(glGetUniformLocation(display_program, "SHADOW_FROM"),
                SHADOW_FROM.x,
                SHADOW_FROM.y,
                SHADOW_FROM.z);
    EXIT_IF_GL_ERROR();

    const u32 shadow_program = get_program(PATH_SHADOW_VERT, PATH_SHADOW_FRAG);
    const u32 depth_map_fbo = get_depth_map_fbo();
    const u32 depth_map = get_depth_map(depth_map_fbo);

    glUniform1f(glGetUniformLocation(shadow_program, "FOV_DEGREES"),
                FOV_DEGREES);
    glUniform1f(glGetUniformLocation(shadow_program, "ASPECT_RATIO"),
                ((f32)WINDOW_WIDTH) / ((f32)WINDOW_HEIGHT));
    glUniform1f(glGetUniformLocation(shadow_program, "VIEW_NEAR"), VIEW_NEAR);
    glUniform1f(glGetUniformLocation(shadow_program, "VIEW_FAR"), VIEW_FAR);
    glUniform3f(glGetUniformLocation(shadow_program, "VIEW_UP"),
                VIEW_UP.x,
                VIEW_UP.y,
                VIEW_UP.z);

    glUniform3f(glGetUniformLocation(shadow_program, "SHADOW_FROM"),
                SHADOW_FROM.x,
                SHADOW_FROM.y,
                SHADOW_FROM.z);
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

            glBufferSubData(GL_ARRAY_BUFFER,
                            0,
                            sizeof(PLAYER_POSITION),
                            &PLAYER_POSITION);
            glUseProgram(shadow_program);
            glUniform3f(glGetUniformLocation(shadow_program, "SHADOW_TO"),
                        SHADOW_TO.x,
                        0.0f,
                        SHADOW_TO.z);
            glCullFace(GL_FRONT);

            glBindFramebuffer(GL_FRAMEBUFFER, depth_map_fbo);
            glClear(GL_DEPTH_BUFFER_BIT);
            glDrawElementsInstanced(GL_TRIANGLES,
                                    sizeof(INDICES) / (sizeof(u8)),
                                    GL_UNSIGNED_BYTE,
                                    (void*)((u64)vbo_index),
                                    (i32)LEN_RECTS);

            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glUseProgram(display_program);
            glCullFace(GL_BACK);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, depth_map);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glUniform3f(glGetUniformLocation(display_program, "VIEW_OFFSET"),
                        VIEW_OFFSET.x,
                        VIEW_OFFSET.y,
                        VIEW_OFFSET.z);
            glUniform3f(glGetUniformLocation(display_program, "SHADOW_TO"),
                        SHADOW_TO.x,
                        0.0f,
                        SHADOW_TO.z);
            glDrawElementsInstanced(GL_TRIANGLES,
                                    sizeof(INDICES) / (sizeof(u8)),
                                    GL_UNSIGNED_BYTE,
                                    (void*)((u64)vbo_index),
                                    (i32)LEN_RECTS);
            EXIT_IF_GL_ERROR();
            glfwSwapBuffers(window);

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
    glDeleteFramebuffers(1, &depth_map_fbo);
    glDeleteTextures(1, &depth_map);
    glDeleteProgram(display_program);
    glDeleteProgram(shadow_program);
    glfwTerminate();
    return OK;
}
