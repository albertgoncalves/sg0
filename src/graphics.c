#include "graphics.h"

#include "image.h"
#include "string.h"
#include "world.h"

#include <sys/mman.h>

typedef struct {
    Mat4 projection;
    Mat4 view;
} Uniforms;

#define PATH_CUBE_VERT "shaders/cube_vert.glsl"
#define PATH_CUBE_FRAG "shaders/cube_frag.glsl"

#define PATH_LINE_VERT "shaders/line_vert.glsl"
#define PATH_LINE_FRAG "shaders/line_frag.glsl"

#define PATH_SPRITE_VERT "shaders/sprite_vert.glsl"
#define PATH_SPRITE_FRAG "shaders/sprite_frag.glsl"

#define PATH_SPRITE_ATLAS "assets/sprite_atlas.png"

#if 1
    #define WINDOW_WIDTH  1500
    #define WINDOW_HEIGHT 860
#else
    #define WINDOW_WIDTH  2500
    #define WINDOW_HEIGHT 1150
#endif

#define FOV_DEGREES  45.0f
#define ASPECT_RATIO (((f32)WINDOW_WIDTH) / ((f32)WINDOW_HEIGHT))

#define VIEW_NEAR 0.01f
#define VIEW_FAR  100.0f
#define VIEW_UP   ((Vec3f){0.0f, 1.0f, 0.0f})

#define VIEW_FROM ((Vec3f){0.0f, 55.0f, 30.0f})
#define VIEW_TO   ((Vec3f){0.0f, 0.0f, 5.0f})

#define CAMERA_LATENCY 175.0f

#define COLOR_BACKGROUND ((Vec4f){0.1f, 0.1f, 0.1f, 1.0f})

#define LINE_WIDTH 3.5f

#define UNIFORM_INDEX 0

#define SPRITE_ATLAS_COLS 9
#define SPRITE_ATLAS_ROWS 8

#define CAP_VAO 3
static u32 VAO[CAP_VAO];

#define CAP_VBO 3
static u32 VBO[CAP_VBO];

#define CAP_EBO 1
static u32 EBO[CAP_EBO];

#define CAP_INSTANCE_VBO 3
static u32 INSTANCE_VBO[CAP_INSTANCE_VBO];

#define CAP_UBO 1
static u32 UBO[CAP_UBO];

#define CAP_TEXTURES 1
static u32 TEXTURES[CAP_TEXTURES];

static Uniforms UNIFORMS;

static u32 PROGRAM_CUBE;
static u32 PROGRAM_LINE;
static u32 PROGRAM_SPRITE;

static const CubeVertex VERTICES_CUBE[] = {
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

static const Vec3u INDICES_CUBE[] = {
    {0, 1, 2},
    {2, 3, 0},
    {4, 5, 6},
    {6, 7, 4},
    {8, 9, 10},
    {10, 11, 8},
    {12, 13, 14},
    {14, 15, 12},
    // NOTE: We can avoid `z-fighting` if we simply don't draw the bottom face
    // of the cube.
    // {16, 17, 18},
    // {18, 19, 16},
    {20, 21, 22},
    {22, 23, 20},
};

static const Vec3f VERTICES_QUAD[] = {
    {0.5f, 0.5f, 0.0f},
    {0.5f, -0.5f, 0.0f},
    {-0.5f, 0.5f, 0.0f},
    {-0.5f, -0.5f, 0.0f},
};

static const Vec3f VERTICES_LINE[] = {
    {-0.5f, -0.5f, -0.5f},
    {0.5f, 0.5f, 0.5f},
};

#define BIND_BUFFER(object, data, size, target, usage) \
    do {                                               \
        glBindBuffer(target, object);                  \
        glBufferData(target, size, data, usage);       \
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
    } while (FALSE)

void graphics_update_camera(Vec3f target) {
    OFFSET_VIEW.x -= (OFFSET_VIEW.x - target.x) / CAMERA_LATENCY;
    OFFSET_VIEW.z -= (OFFSET_VIEW.z - target.z) / CAMERA_LATENCY;
}

ATTRIBUTE(noreturn)
static void callback_glfw_error(i32 code, const char* error) {
    fflush(stdout);
    fflush(stderr);
    fprintf(stderr, "%d", code);
    if (error) {
        fprintf(stderr, ": %s", error);
    }
    fputc('\n', stderr);
    EXIT();
}

ATTRIBUTE(noreturn)
static void callback_gl_debug(u32,
                              u32,
                              u32,
                              u32,
                              i32         length,
                              const char* message,
                              const void*) {
    fflush(stdout);
    fflush(stderr);
    if (0 < length) {
        fprintf(stderr, "%.*s", length, message);
    } else {
        fprintf(stderr, "%s", message);
    }
    EXIT();
}

GLFWwindow* graphics_window(void) {
    glfwSetErrorCallback(callback_glfw_error);

    EXIT_IF(!glfwInit());

    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, TRUE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, FALSE);
    glfwWindowHint(GLFW_SAMPLES, 16);
    GLFWwindow* window =
        glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, __FILE__, NULL, NULL);
    EXIT_IF(!window);

    glfwMakeContextCurrent(window);
    glfwSwapInterval(VSYNC);

    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(callback_gl_debug, NULL);

    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    glClearColor(COLOR_BACKGROUND.x,
                 COLOR_BACKGROUND.y,
                 COLOR_BACKGROUND.z,
                 COLOR_BACKGROUND.w);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    return window;
}

static void compile_shader(const char* path, u32 shader) {
    const MemMap map = string_open(path);
    const char*  source = string_copy(map);
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

static u32 compile_program(const char* source_vert, const char* source_frag) {
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
    return program;
}

static void cubes_init(void) {
    PROGRAM_CUBE = compile_program(PATH_CUBE_VERT, PATH_CUBE_FRAG);
    glUseProgram(PROGRAM_CUBE);
    glBindVertexArray(VAO[0]);

    BIND_BUFFER(VBO[0],
                VERTICES_CUBE,
                sizeof(VERTICES_CUBE),
                GL_ARRAY_BUFFER,
                GL_STATIC_DRAW);

#define SIZE   (sizeof(Vec3f) / sizeof(f32))
#define STRIDE sizeof(VERTICES_CUBE[0])
    SET_VERTEX_ATTRIB(PROGRAM_CUBE,
                      "VERT_IN_POSITION",
                      SIZE,
                      STRIDE,
                      offsetof(CubeVertex, position));
    SET_VERTEX_ATTRIB(PROGRAM_CUBE,
                      "VERT_IN_NORMAL",
                      SIZE,
                      STRIDE,
                      offsetof(CubeVertex, normal));
#undef SIZE
#undef STRIDE

    BIND_BUFFER(EBO[0],
                INDICES_CUBE,
                sizeof(INDICES_CUBE),
                GL_ELEMENT_ARRAY_BUFFER,
                GL_STATIC_DRAW);
    BIND_BUFFER(INSTANCE_VBO[0],
                CUBES,
                sizeof(CUBES),
                GL_ARRAY_BUFFER,
                GL_DYNAMIC_DRAW);

#define SIZE   (sizeof(Vec3f) / sizeof(f32))
#define STRIDE sizeof(CUBES[0])
    SET_VERTEX_ATTRIB_DIV(PROGRAM_CUBE,
                          "VERT_IN_TRANSLATE",
                          SIZE,
                          STRIDE,
                          offsetof(Geom, translate));
    SET_VERTEX_ATTRIB_DIV(PROGRAM_CUBE,
                          "VERT_IN_SCALE",
                          SIZE,
                          STRIDE,
                          offsetof(Geom, scale));
#undef SIZE
    SET_VERTEX_ATTRIB_DIV(PROGRAM_CUBE,
                          "VERT_IN_COLOR",
                          sizeof(Vec4f) / sizeof(f32),
                          STRIDE,
                          offsetof(Geom, color));
#undef STRIDE

    glUniform3f(glGetUniformLocation(PROGRAM_CUBE, "VIEW_FROM"),
                VIEW_FROM.x,
                VIEW_FROM.y,
                VIEW_FROM.z);
    glUniformBlockBinding(PROGRAM_CUBE,
                          glGetUniformBlockIndex(PROGRAM_CUBE, "MATRICES"),
                          UNIFORM_INDEX);
}

static void lines_init(void) {
    PROGRAM_LINE = compile_program(PATH_LINE_VERT, PATH_LINE_FRAG);
    glUseProgram(PROGRAM_LINE);

    glLineWidth(LINE_WIDTH);
    glEnable(GL_LINE_SMOOTH);

    glBindVertexArray(VAO[1]);

    BIND_BUFFER(VBO[1],
                VERTICES_LINE,
                sizeof(VERTICES_LINE),
                GL_ARRAY_BUFFER,
                GL_STATIC_DRAW);
    SET_VERTEX_ATTRIB(PROGRAM_LINE,
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
    SET_VERTEX_ATTRIB_DIV(PROGRAM_LINE,
                          "VERT_IN_TRANSLATE",
                          SIZE,
                          STRIDE,
                          offsetof(Geom, translate));
    SET_VERTEX_ATTRIB_DIV(PROGRAM_LINE,
                          "VERT_IN_SCALE",
                          SIZE,
                          STRIDE,
                          offsetof(Geom, scale));
#undef SIZE
    SET_VERTEX_ATTRIB_DIV(PROGRAM_LINE,
                          "VERT_IN_COLOR",
                          sizeof(Vec4f) / sizeof(f32),
                          STRIDE,
                          offsetof(Geom, color));
#undef STRIDE

    glUniformBlockBinding(PROGRAM_LINE,
                          glGetUniformBlockIndex(PROGRAM_LINE, "MATRICES"),
                          UNIFORM_INDEX);
}

static void sprites_init(void) {
    {
        const Image sprite_player = image_open(PATH_SPRITE_ATLAS);
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
    }

    PROGRAM_SPRITE = compile_program(PATH_SPRITE_VERT, PATH_SPRITE_FRAG);
    glUseProgram(PROGRAM_SPRITE);
    glBindVertexArray(VAO[2]);

    BIND_BUFFER(VBO[2],
                VERTICES_QUAD,
                sizeof(VERTICES_QUAD),
                GL_ARRAY_BUFFER,
                GL_STATIC_DRAW);
    SET_VERTEX_ATTRIB(PROGRAM_SPRITE,
                      "VERT_IN_POSITION",
                      sizeof(Vec3f) / sizeof(f32),
                      sizeof(VERTICES_QUAD[0]),
                      offsetof(Vec3f, x));
    BIND_BUFFER(INSTANCE_VBO[2],
                SPRITES,
                sizeof(SPRITES),
                GL_ARRAY_BUFFER,
                GL_DYNAMIC_DRAW);

#define SIZE   (sizeof(Vec3f) / sizeof(f32))
#define STRIDE sizeof(SPRITES[0])
    SET_VERTEX_ATTRIB_DIV(PROGRAM_SPRITE,
                          "VERT_IN_TRANSLATE",
                          SIZE,
                          STRIDE,
                          offsetof(Sprite, geom) + offsetof(Geom, translate));
    SET_VERTEX_ATTRIB_DIV(PROGRAM_SPRITE,
                          "VERT_IN_SCALE",
                          SIZE,
                          STRIDE,
                          offsetof(Sprite, geom) + offsetof(Geom, scale));
#undef SIZE
    SET_VERTEX_ATTRIB_DIV(PROGRAM_SPRITE,
                          "VERT_IN_COLOR",
                          sizeof(Vec4f) / sizeof(f32),
                          STRIDE,
                          offsetof(Sprite, geom) + offsetof(Geom, color));
    {
        const u32 index =
            (u32)glGetAttribLocation(PROGRAM_SPRITE, "VERT_IN_COL_ROW");
        glEnableVertexAttribArray(index);
        glVertexAttribIPointer(index,
                               sizeof(Vec2u) / sizeof(u8),
                               GL_UNSIGNED_BYTE,
                               STRIDE,
                               (void*)offsetof(Sprite, col_row));
        glVertexAttribDivisor(index, 1);
    }
#undef STRIDE

    glUniform2ui(glGetUniformLocation(PROGRAM_SPRITE, "COLS_ROWS"),
                 SPRITE_ATLAS_COLS,
                 SPRITE_ATLAS_ROWS);
    glUniformBlockBinding(PROGRAM_SPRITE,
                          glGetUniformBlockIndex(PROGRAM_SPRITE, "MATRICES"),
                          UNIFORM_INDEX);
}

void graphics_init(void) {
    glGenVertexArrays(CAP_VAO, &VAO[0]);
    glGenBuffers(CAP_VBO, &VBO[0]);
    glGenBuffers(CAP_EBO, &EBO[0]);
    glGenBuffers(CAP_INSTANCE_VBO, &INSTANCE_VBO[0]);
    glGenBuffers(CAP_UBO, &UBO[0]);
    glGenTextures(CAP_TEXTURES, &TEXTURES[0]);

    UNIFORMS.projection =
        math_perspective(FOV_DEGREES, ASPECT_RATIO, VIEW_NEAR, VIEW_FAR);
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

    cubes_init();
    lines_init();
    sprites_init();
}

void graphics_free(void) {
    glDeleteVertexArrays(CAP_VAO, &VAO[0]);
    glDeleteBuffers(CAP_VBO, &VBO[0]);
    glDeleteBuffers(CAP_EBO, &EBO[0]);
    glDeleteBuffers(CAP_INSTANCE_VBO, &INSTANCE_VBO[0]);
    glDeleteBuffers(CAP_UBO, &UBO[0]);
    glDeleteTextures(CAP_TEXTURES, &TEXTURES[0]);

    glDeleteProgram(PROGRAM_CUBE);
    glDeleteProgram(PROGRAM_LINE);
    glDeleteProgram(PROGRAM_SPRITE);
}

void graphics_update_uniforms(void) {
    const Vec3f view_from = (Vec3f){
        .x = VIEW_FROM.x + OFFSET_VIEW.x,
        .y = VIEW_FROM.y + OFFSET_VIEW.y,
        .z = VIEW_FROM.z + OFFSET_VIEW.z,
    };
    const Vec3f view_to = (Vec3f){
        .x = VIEW_TO.x + OFFSET_VIEW.x,
        .y = VIEW_TO.y + OFFSET_VIEW.y,
        .z = VIEW_TO.z + OFFSET_VIEW.z,
    };
    UNIFORMS.view = math_look_at(view_from, view_to, VIEW_UP);
}

void graphics_draw(GLFWwindow* window) {
    glBindBuffer(GL_UNIFORM_BUFFER, UBO[0]);
    glBufferSubData(GL_UNIFORM_BUFFER,
                    offsetof(Uniforms, view),
                    sizeof(Mat4),
                    &UNIFORMS.view.column_row[0][0]);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glDisable(GL_MULTISAMPLE);
    glUseProgram(PROGRAM_SPRITE);
    glBindVertexArray(VAO[2]);
    glBindBuffer(GL_ARRAY_BUFFER, INSTANCE_VBO[2]);
    glBufferSubData(GL_ARRAY_BUFFER,
                    0,
                    sizeof(Sprite) * LEN_SPRITES,
                    &SPRITES[0]);
    glDrawArraysInstanced(GL_TRIANGLE_STRIP,
                          0,
                          sizeof(VERTICES_QUAD) / sizeof(VERTICES_QUAD[0]),
                          (i32)LEN_SPRITES);

    glEnable(GL_MULTISAMPLE);
    glUseProgram(PROGRAM_CUBE);
    glBindVertexArray(VAO[0]);
    glBindBuffer(GL_ARRAY_BUFFER, INSTANCE_VBO[0]);
    // NOTE: We don't need to sync the stationary geometry every frame. This
    // could be more efficient.
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Geom) * LEN_CUBES, &CUBES[0]);
    glDrawElementsInstanced(GL_TRIANGLES,
                            sizeof(INDICES_CUBE) / (sizeof(u8)),
                            GL_UNSIGNED_BYTE,
                            NULL,
                            (i32)LEN_CUBES);

    glUseProgram(PROGRAM_LINE);
    glBindVertexArray(VAO[1]);
    glBindBuffer(GL_ARRAY_BUFFER, INSTANCE_VBO[1]);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Geom) * LEN_LINES, &LINES[0]);
    glDrawArraysInstanced(GL_LINES,
                          0,
                          sizeof(VERTICES_LINE) / sizeof(VERTICES_LINE[0]),
                          (i32)LEN_LINES);

    glfwSwapBuffers(window);
}
