#version 440 core

layout(location=0) in ivec4 vInds;

out ivec4 inds;

void main() {
    inds = vInds;

    gl_Position = vec4(0, 0, 0, 1);
}
