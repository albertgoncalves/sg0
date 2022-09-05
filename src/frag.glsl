#version 330 core

in vec3 VERT_OUT_NORMAL;
in vec3 VERT_OUT_COLOR;

layout(location = 0) out vec4 FRAG_OUT_COLOR;

void main() {
    FRAG_OUT_COLOR =
        vec4(mix(VERT_OUT_COLOR, (VERT_OUT_NORMAL + 1.0f) / 2.0f, 0.5f), 1.0f);
}
