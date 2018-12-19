#version 440 core

layout(location=0) in vec4 pos4;

layout(std140, binding=1) uniform Matrices {
    mat4 model;
    mat4 view;
    mat4 proj;
};

out vec4 pos;

void main() {
    pos = model * pos4;
    gl_Position = proj * view * vec4(pos.xyz, -pos.w + 2);
}
