#version 330 core

layout(location = 0) in vec3 VERT_IN_POSITION;
layout(location = 1) in vec3 VERT_IN_NORMAL;

uniform float FOV_DEGREES;
uniform float ASPECT_RATIO;
uniform float VIEW_NEAR;
uniform float VIEW_FAR;

uniform vec3 VIEW_FROM;
uniform vec3 VIEW_TO;
uniform vec3 VIEW_UP;
uniform vec3 VIEW_OFFSET;

uniform vec3 POSITION;

out vec3 VERT_OUT_COLOR;

#define PI 3.1415926535897932385f

mat4 translate(vec3 v) {
    mat4 m = mat4(0.0f);
    m[0][0] = 1.0f;
    m[1][1] = 1.0f;
    m[2][2] = 1.0f;
    m[3][3] = 1.0f;
    m[3][0] = v.x;
    m[3][1] = v.y;
    m[3][2] = v.z;
    return m;
}

mat4 perspective(float fov_degrees,
                 float aspect_ratio,
                 float view_near,
                 float view_far) {
    float cotangent = 1.0f / tan(radians(fov_degrees) / 2.0f);
    float d = view_near - view_far;
    mat4  m = mat4(0.0f);
    m[0][0] = cotangent / aspect_ratio;
    m[1][1] = cotangent;
    m[2][3] = -1.0f;
    m[2][2] = (view_near + view_far) / d;
    m[3][2] = (view_near * view_far * 2.0f) / d;
    m[3][3] = 0.0f;
    return m;
}

mat4 look_at(vec3 view_from, vec3 view_to, vec3 up) {
    vec3 f = normalize(view_to - view_from);
    vec3 s = normalize(cross(f, up));
    vec3 u = cross(s, f);
    mat4 m = mat4(0.0f);
    m[0][0] = s.x;
    m[0][1] = u.x;
    m[0][2] = -f.x;
    m[0][3] = 0.0f;
    m[1][0] = s.y;
    m[1][1] = u.y;
    m[1][2] = -f.y;
    m[1][3] = 0.0f;
    m[2][0] = s.z;
    m[2][1] = u.z;
    m[2][2] = -f.z;
    m[2][3] = 0.0f;
    m[3][0] = -dot(s, view_from);
    m[3][1] = -dot(u, view_from);
    m[3][2] = dot(f, view_from);
    m[3][3] = 1.0f;
    return m;
}

void main() {
    mat4 view =
        look_at(VIEW_FROM + VIEW_OFFSET, VIEW_TO + VIEW_OFFSET, VIEW_UP);
    mat4 projection =
        perspective(FOV_DEGREES, ASPECT_RATIO, VIEW_NEAR, VIEW_FAR);
    gl_Position =
        projection * view * translate(POSITION) * vec4(VERT_IN_POSITION, 1.0f);
    VERT_OUT_COLOR = (VERT_IN_NORMAL + 1.0f) / 2.0f;
}
