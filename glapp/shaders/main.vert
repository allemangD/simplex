#version 440 core

layout(location=0) in vec3 vCol;
layout(location=1) in vec2 vPos;

smooth out vec3 color;

layout(std140, binding=3) uniform Matrices {
    mat4 proj;
    mat4 view;
    mat4 model;
};

void main() {
    gl_Position = proj * view * model * vec4(vPos, 0, 1);
    color = vCol;
}