#ifndef __GRAPHICS_H__
#define __GRAPHICS_H__

#include "camera.h"
#include "image.h"
#include "memory.h"
#include "string.h"

#define GL_GLEXT_PROTOTYPES

#include <GLFW/glfw3.h>

#define BACKGROUND_COLOR 0.1f, 0.1f, 0.1f, 1.0f

#define PATH_CUBE_VERT "src/cube_vert.glsl"
#define PATH_CUBE_FRAG "src/cube_frag.glsl"

#define PATH_LINE_VERT "src/line_vert.glsl"
#define PATH_LINE_FRAG "src/line_frag.glsl"

#define PATH_SPRITE_VERT "src/sprite_vert.glsl"
#define PATH_SPRITE_FRAG "src/sprite_frag.glsl"

typedef struct {
    Mat4 projection;
    Mat4 view;
} Uniforms;

#define UNIFORM_INDEX 0

#define CAP_VAO 3
static u32 VAO[CAP_VAO];

#define CAP_VBO 3
static u32 VBO[CAP_VBO];

#define CAP_EBO 2
static u32 EBO[CAP_EBO];

#define CAP_INSTANCE_VBO 3
static u32 INSTANCE_VBO[CAP_INSTANCE_VBO];

#define CAP_UBO 1
static u32 UBO[CAP_UBO];

#define CAP_TEXTURES 1
static u32 TEXTURES[CAP_TEXTURES];

static Uniforms UNIFORMS;

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

static GLFWwindow* init_window(void) {
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
    return window;
}

static void init_graphics(void) {
    glGenVertexArrays(CAP_VAO, &VAO[0]);
    glGenBuffers(CAP_VBO, &VBO[0]);
    glGenBuffers(CAP_EBO, &EBO[0]);
    glGenBuffers(CAP_INSTANCE_VBO, &INSTANCE_VBO[0]);
    glGenBuffers(CAP_UBO, &UBO[0]);
    glGenTextures(CAP_TEXTURES, &TEXTURES[0]);
    EXIT_IF_GL_ERROR();
}

static void free_graphics(void) {
    glDeleteVertexArrays(CAP_VAO, &VAO[0]);
    glDeleteBuffers(CAP_VBO, &VBO[0]);
    glDeleteBuffers(CAP_EBO, &EBO[0]);
    glDeleteBuffers(CAP_INSTANCE_VBO, &INSTANCE_VBO[0]);
    glDeleteBuffers(CAP_UBO, &UBO[0]);
    glDeleteTextures(CAP_TEXTURES, &TEXTURES[0]);
    EXIT_IF_GL_ERROR();
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

#define BIND_BUFFER(object, data, size, target, usage) \
    do {                                               \
        glBindBuffer(target, object);                  \
        glBufferData(target, size, data, usage);       \
        EXIT_IF_GL_ERROR();                            \
    } while (FALSE)

#define SET_VERTEX_ATTRIB(program, label, size, stride, offset)     \
    do {                                                            \
        const u32 index = (u32)glGetAttribLocation(program, label); \
        glEnableVertexAttribArray(index);                           \
        glVertexAttribPointer(index,                                \
                              size,                                 \
                              GL_FLOAT,                             \
                              FALSE,                                \
                              stride,                               \
                              (void*)(offset));                     \
        EXIT_IF_GL_ERROR();                                         \
    } while (FALSE)

#define SET_VERTEX_ATTRIB_DIV(program, label, size, stride, offset) \
    do {                                                            \
        const u32 index = (u32)glGetAttribLocation(program, label); \
        glEnableVertexAttribArray(index);                           \
        glVertexAttribPointer(index,                                \
                              size,                                 \
                              GL_FLOAT,                             \
                              FALSE,                                \
                              stride,                               \
                              (void*)(offset));                     \
        glVertexAttribDivisor(index, 1);                            \
        EXIT_IF_GL_ERROR();                                         \
    } while (FALSE)

static u32 init_cubes(void) {
    const u32 cube_program = get_program(PATH_CUBE_VERT, PATH_CUBE_FRAG);
    glUseProgram(cube_program);
    glBindVertexArray(VAO[0]);

    BIND_BUFFER(VBO[0],
                CUBE_VERTICES,
                sizeof(CUBE_VERTICES),
                GL_ARRAY_BUFFER,
                GL_STATIC_DRAW);

#define SIZE   (sizeof(Vec3f) / sizeof(f32))
#define STRIDE sizeof(CUBE_VERTICES[0])
    SET_VERTEX_ATTRIB(cube_program,
                      "VERT_IN_POSITION",
                      SIZE,
                      STRIDE,
                      offsetof(CubeVertex, position));
    SET_VERTEX_ATTRIB(cube_program,
                      "VERT_IN_NORMAL",
                      SIZE,
                      STRIDE,
                      offsetof(CubeVertex, normal));
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
    SET_VERTEX_ATTRIB_DIV(cube_program,
                          "VERT_IN_TRANSLATE",
                          SIZE,
                          STRIDE,
                          offsetof(Geom, translate));
    SET_VERTEX_ATTRIB_DIV(cube_program,
                          "VERT_IN_SCALE",
                          SIZE,
                          STRIDE,
                          offsetof(Geom, scale));
#undef SIZE
    SET_VERTEX_ATTRIB_DIV(cube_program,
                          "VERT_IN_COLOR",
                          sizeof(Vec4f) / sizeof(f32),
                          STRIDE,
                          offsetof(Geom, color));
#undef STRIDE

    glUniform3f(glGetUniformLocation(cube_program, "VIEW_FROM"),
                VIEW_FROM.x,
                VIEW_FROM.y,
                VIEW_FROM.z);
    glUniformBlockBinding(cube_program,
                          glGetUniformBlockIndex(cube_program, "MATRICES"),
                          UNIFORM_INDEX);
    EXIT_IF_GL_ERROR();
    return cube_program;
}

static u32 init_lines(void) {
    const u32 line_program = get_program(PATH_LINE_VERT, PATH_LINE_FRAG);
    glUseProgram(line_program);
    glBindVertexArray(VAO[1]);

    BIND_BUFFER(VBO[1],
                LINE_VERTICES,
                sizeof(LINE_VERTICES),
                GL_ARRAY_BUFFER,
                GL_STATIC_DRAW);
    SET_VERTEX_ATTRIB(line_program,
                      "VERT_IN_POSITION",
                      sizeof(Vec3f) / sizeof(f32),
                      sizeof(Vec3f),
                      offsetof(Vec3f, x));
    BIND_BUFFER(INSTANCE_VBO[1],
                LINES,
                sizeof(LINES),
                GL_ARRAY_BUFFER,
                GL_DYNAMIC_DRAW);

#define SIZE   (sizeof(Vec3f) / sizeof(f32))
#define STRIDE sizeof(LINES[0])
    SET_VERTEX_ATTRIB_DIV(line_program,
                          "VERT_IN_TRANSLATE",
                          SIZE,
                          STRIDE,
                          offsetof(Geom, translate));
    SET_VERTEX_ATTRIB_DIV(line_program,
                          "VERT_IN_SCALE",
                          SIZE,
                          STRIDE,
                          offsetof(Geom, scale));
#undef SIZE
    SET_VERTEX_ATTRIB_DIV(line_program,
                          "VERT_IN_COLOR",
                          sizeof(Vec4f) / sizeof(f32),
                          STRIDE,
                          offsetof(Geom, color));
#undef STRIDE

    glUniformBlockBinding(line_program,
                          glGetUniformBlockIndex(line_program, "MATRICES"),
                          UNIFORM_INDEX);
    EXIT_IF_GL_ERROR();
    return line_program;
}

static u32 init_sprites(void) {
    {
        const Image sprite_player = image_rgba_from_path(PATH_SPRITE_ATLAS);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, TEXTURES[0]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexImage2D(GL_TEXTURE_2D,
                     0,
                     GL_RGBA,
                     sprite_player.width,
                     sprite_player.height,
                     0,
                     GL_RGBA,
                     GL_UNSIGNED_BYTE,
                     sprite_player.pixels);
        image_free(sprite_player);
        EXIT_IF_GL_ERROR();
    }

    const u32 sprite_program = get_program(PATH_SPRITE_VERT, PATH_SPRITE_FRAG);
    glUseProgram(sprite_program);
    glBindVertexArray(VAO[2]);

    BIND_BUFFER(VBO[2],
                QUAD_VERTICES,
                sizeof(QUAD_VERTICES),
                GL_ARRAY_BUFFER,
                GL_STATIC_DRAW);
    SET_VERTEX_ATTRIB(sprite_program,
                      "VERT_IN_POSITION",
                      sizeof(Vec3f) / sizeof(f32),
                      sizeof(QUAD_VERTICES[0]),
                      offsetof(Vec3f, x));
    BIND_BUFFER(EBO[1],
                QUAD_INDICES,
                sizeof(QUAD_INDICES),
                GL_ELEMENT_ARRAY_BUFFER,
                GL_STATIC_DRAW);
    BIND_BUFFER(INSTANCE_VBO[2],
                SPRITES,
                sizeof(SPRITES),
                GL_ARRAY_BUFFER,
                GL_DYNAMIC_DRAW);

#define SIZE   (sizeof(Vec3f) / sizeof(f32))
#define STRIDE sizeof(SPRITES[0])
    SET_VERTEX_ATTRIB_DIV(sprite_program,
                          "VERT_IN_TRANSLATE",
                          SIZE,
                          STRIDE,
                          offsetof(Sprite, geom) + offsetof(Geom, translate));
    SET_VERTEX_ATTRIB_DIV(sprite_program,
                          "VERT_IN_SCALE",
                          SIZE,
                          STRIDE,
                          offsetof(Sprite, geom) + offsetof(Geom, scale));
#undef SIZE
    SET_VERTEX_ATTRIB_DIV(sprite_program,
                          "VERT_IN_COLOR",
                          sizeof(Vec4f) / sizeof(f32),
                          STRIDE,
                          offsetof(Sprite, geom) + offsetof(Geom, color));
    {
        const u32 index =
            (u32)glGetAttribLocation(sprite_program, "VERT_IN_COL_ROW");
        glEnableVertexAttribArray(index);
        glVertexAttribIPointer(index,
                               sizeof(Vec2u) / sizeof(u8),
                               GL_UNSIGNED_BYTE,
                               STRIDE,
                               (void*)offsetof(Sprite, col_row));
        glVertexAttribDivisor(index, 1);
        EXIT_IF_GL_ERROR();
    }
#undef STRIDE

    glUniform2ui(glGetUniformLocation(sprite_program, "COLS_ROWS"),
                 SPRITE_ATLAS_COLS,
                 SPRITE_ATLAS_ROWS);
    glUniformBlockBinding(sprite_program,
                          glGetUniformBlockIndex(sprite_program, "MATRICES"),
                          UNIFORM_INDEX);
    EXIT_IF_GL_ERROR();
    return sprite_program;
}

static void init_uniforms(void) {
    UNIFORMS.projection =
        perspective(FOV_DEGREES, ASPECT_RATIO, VIEW_NEAR, VIEW_FAR);
    BIND_BUFFER(UBO[0],
                &UNIFORMS,
                sizeof(Uniforms),
                GL_UNIFORM_BUFFER,
                GL_DYNAMIC_DRAW);
    glBindBufferRange(GL_UNIFORM_BUFFER,
                      UNIFORM_INDEX,
                      UBO[0],
                      0,
                      sizeof(Uniforms));
    EXIT_IF_GL_ERROR();
}

static void update_uniforms(void) {
    UNIFORMS.view = get_view();
}

static void draw(GLFWwindow* window,
                 u32         cube_program,
                 u32         line_program,
                 u32         sprite_program) {
    glBindBuffer(GL_UNIFORM_BUFFER, UBO[0]);
    glBufferSubData(GL_UNIFORM_BUFFER,
                    offsetof(Uniforms, view),
                    sizeof(Mat4),
                    &UNIFORMS.view.column_row[0][0]);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(sprite_program);
    glBindVertexArray(VAO[2]);
    glBindBuffer(GL_ARRAY_BUFFER, INSTANCE_VBO[2]);
    glBufferSubData(GL_ARRAY_BUFFER,
                    0,
                    sizeof(Sprite) * LEN_SPRITES,
                    &SPRITES[0]);
    glDrawElementsInstanced(GL_TRIANGLES,
                            sizeof(QUAD_INDICES) / (sizeof(u8)),
                            GL_UNSIGNED_BYTE,
                            NULL,
                            (i32)LEN_SPRITES);

    glUseProgram(line_program);
    glBindVertexArray(VAO[1]);
    glBindBuffer(GL_ARRAY_BUFFER, INSTANCE_VBO[1]);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Geom) * LEN_LINES, &LINES[0]);
    glDrawArraysInstanced(GL_LINES, 0, 2, (i32)LEN_LINES);

    glUseProgram(cube_program);
    glBindVertexArray(VAO[0]);
    glBindBuffer(GL_ARRAY_BUFFER, INSTANCE_VBO[0]);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Geom) * LEN_CUBES, &CUBES[0]);
    glDrawElementsInstanced(GL_TRIANGLES,
                            sizeof(CUBE_INDICES) / (sizeof(u8)),
                            GL_UNSIGNED_BYTE,
                            NULL,
                            (i32)LEN_CUBES);

    glfwSwapBuffers(window);
    EXIT_IF_GL_ERROR();
}

#endif
