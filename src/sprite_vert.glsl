#version 330 core

layout(location = 0) in vec3 VERT_IN_POSITION;
layout(location = 1) in vec3 VERT_IN_TRANSLATE;
layout(location = 2) in vec3 VERT_IN_SCALE;
layout(location = 3) in vec4 VERT_IN_COLOR;
layout(location = 4) in uvec2 VERT_IN_CELL;

layout(std140) uniform MATRICES {
    mat4 PROJECTION;
    mat4 VIEW;
};

out vec2       VERT_OUT_POSITION;
flat out vec4  VERT_OUT_COLOR;
flat out uvec2 VERT_OUT_CELL;

void main() {
    gl_Position =
        PROJECTION * VIEW *
        vec4((VERT_IN_SCALE * VERT_IN_POSITION) + VERT_IN_TRANSLATE, 1.0f);
    VERT_OUT_POSITION = (VERT_IN_POSITION.xy * vec2(1.0f, -1.0f)) + 0.5f;
    VERT_OUT_COLOR = VERT_IN_COLOR;
    VERT_OUT_CELL = VERT_IN_CELL;
}
