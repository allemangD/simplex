#version 440 core

layout(points) in;
layout(line_strip, max_vertices=48) out;

layout(std430, binding=1) buffer Positions {
    vec4 verts[];
};

layout(std140, binding=1) uniform Matrices {
    mat4 model;
    vec4 offset;

    mat4 view;
    mat4 proj;
};

in ivec4 inds[];

out vec4 pos;

void emit(vec4 v) {
    pos = v;
    gl_Position = proj * view * vec4(v.xyz, 1 - v.w / 2);
    EmitVertex();
}

void main() {
    vec4 pos4[4];
    for(int i = 0; i < 4; ++i) pos4[i] = offset + model * verts[inds[0][i]];

    for(int i = 0; i < 4; ++i) {
        for(int j = i + 1; j < 4; ++j) {
            emit(pos4[i]);
            emit(pos4[j]);
            EndPrimitive();
        }
    }
}
