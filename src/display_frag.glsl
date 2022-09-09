#version 330 core

layout(location = 0) out vec4 FRAG_OUT_COLOR;

in vec3 VERT_OUT_POSITION;
in vec3 VERT_OUT_NORMAL;
in vec4 VERT_OUT_COLOR;
in vec4 VERT_OUT_SHADOW_POS;

uniform vec3 VIEW_FROM;

uniform vec3 SHADOW_FROM;
uniform vec3 SHADOW_TO;

uniform sampler2D SHADOW_MAP;

#define BIAS_MIN 0.00001f
#define BIAS_MAX 0.0001f

bool shadow(vec4 position, vec3 normal) {
    vec3  point = ((position.xyz / position.w) * 0.5f) + 0.5f;
    vec3  direction = SHADOW_TO - SHADOW_FROM;
    float bias = max(BIAS_MAX * (1.0f - dot(normal, direction)), BIAS_MIN);
    // return (point.z - bias) > texture(SHADOW_MAP, point.xy).r ? 0.5f : 1.0f;
    return (point.z - bias) > texture(SHADOW_MAP, point.xy).r;
}

void main() {
    vec3  direction = normalize(VERT_OUT_POSITION + VIEW_FROM);
    float brightness = mix(dot(VERT_OUT_NORMAL, direction), 1.0f, 0.75f);
    if (shadow(VERT_OUT_SHADOW_POS, VERT_OUT_NORMAL)) {
        FRAG_OUT_COLOR =
            vec4(VERT_OUT_COLOR.rgb * brightness, VERT_OUT_COLOR.a);
    } else {
        FRAG_OUT_COLOR = vec4(1.0f, 0.0f, 0.0f, VERT_OUT_COLOR.a);
    }
}
