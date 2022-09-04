#version 330 core

in vec3 VERT_OUT_COLOR;

layout(location = 0) out vec4 FRAG_OUT_COLOR;

void main() {
    FRAG_OUT_COLOR = vec4(VERT_OUT_COLOR, 1.0f);
}
