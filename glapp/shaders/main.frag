#version 440 core

smooth in vec3 color;

out vec4 fcolor;

void main() {
    fcolor = vec4(color, 1);
}