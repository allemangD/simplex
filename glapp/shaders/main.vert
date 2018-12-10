#version 330

uniform mat4 pvm;

in vec3 vCol;
in vec2 vPos;

smooth out vec3 color;

void main() {
    gl_Position = pvm * vec4(vPos, 0, 1);
    color = vCol;
}