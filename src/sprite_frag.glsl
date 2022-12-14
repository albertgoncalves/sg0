#version 330 core

layout(location = 0) out vec4 FRAG_OUT_COLOR;

in vec2       VERT_OUT_POSITION;
flat in vec4  VERT_OUT_COLOR;
flat in uvec2 VERT_OUT_COL_ROW;

uniform sampler2D TEXTURE;
uniform uvec2     COLS_ROWS;

#define EPSILON 0.1f

void main() {
    vec4 pixel =
        texture(TEXTURE, (VERT_OUT_COL_ROW + VERT_OUT_POSITION) / COLS_ROWS);
    if (pixel.a < EPSILON) {
        discard;
    }
    FRAG_OUT_COLOR = VERT_OUT_COLOR;
}
