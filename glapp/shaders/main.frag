#version 330

in vec3 color;

out vec4 fcolor;

void main() {
    fcolor = vec4(color, 1);
}