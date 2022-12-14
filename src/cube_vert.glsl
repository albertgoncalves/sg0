#version 330 core

layout(location = 0) in vec3 VERT_IN_POSITION;
layout(location = 1) in vec3 VERT_IN_NORMAL;
layout(location = 2) in vec3 VERT_IN_TRANSLATE;
layout(location = 3) in vec3 VERT_IN_SCALE;
layout(location = 4) in vec4 VERT_IN_COLOR;

layout(std140) uniform MATRICES {
    mat4 PROJECTION;
    mat4 VIEW;
};

flat out vec3 VERT_OUT_POSITION;
flat out vec3 VERT_OUT_NORMAL;
flat out vec4 VERT_OUT_COLOR;

void main() {
    gl_Position =
        PROJECTION * VIEW *
        vec4((VERT_IN_SCALE * VERT_IN_POSITION) + VERT_IN_TRANSLATE, 1.0f);
    VERT_OUT_POSITION = VERT_IN_POSITION;
    VERT_OUT_NORMAL = VERT_IN_NORMAL;
    VERT_OUT_COLOR = VERT_IN_COLOR;
}
