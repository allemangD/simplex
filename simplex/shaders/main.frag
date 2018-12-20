#version 440 core

out vec4 fcolor;

in vec4 pos;

vec3 hsv2rgb(vec3 c) {
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

void main() {
    float h = pos.w / 3 + .6;
    float s = 1;
    float v = smoothstep(1, -1, pos.z);

    fcolor = vec4(hsv2rgb(vec3(h, s, v)), 1);
}