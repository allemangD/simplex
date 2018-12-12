#version 440 core

uniform mat4 pvm;

layout(location=0) in vec3 vCol;
layout(location=1) in vec2 vPos;

smooth out vec3 color;

void main() {
    gl_Position = pvm * vec4(vPos, 0, 1);
    color = vCol;
}