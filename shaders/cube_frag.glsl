#version 330 core

layout(location = 0) out vec4 FRAG_OUT_COLOR;

flat in vec3 VERT_OUT_POSITION;
flat in vec3 VERT_OUT_NORMAL;
flat in vec4 VERT_OUT_COLOR;

uniform vec3 VIEW_FROM;

void main() {
    vec3  direction = normalize(VERT_OUT_POSITION + VIEW_FROM);
    float brightness = mix(dot(VERT_OUT_NORMAL, direction), 1.0f, 0.75f);
    FRAG_OUT_COLOR = vec4(VERT_OUT_COLOR.rgb * brightness, VERT_OUT_COLOR.a);
}
