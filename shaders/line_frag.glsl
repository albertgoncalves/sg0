#version 330 core

layout(location = 0) out vec4 FRAG_OUT_COLOR;

flat in vec4 VERT_OUT_COLOR;

void main() {
    FRAG_OUT_COLOR = VERT_OUT_COLOR;
}
