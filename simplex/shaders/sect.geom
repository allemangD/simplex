#version 440 core

layout(points) in;
layout(triangle_strip, max_vertices=4) out;

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
    gl_Position = proj * view * vec4(v.xyz, 1);
    EmitVertex();
}

void main() {
    vec4 pos4[4];
    for(int i = 0; i < 4; ++i) pos4[i] = offset + model * verts[inds[0][i]];

    int lo[4], L = 0;
    int hi[4], H = 0;

    for(int i = 0; i < 4; ++i) {
        if (pos4[i].w < 0) {
            lo[L++] = i;
        } else {
            hi[H++] = i;
        }
    }

    vec4 sect[4]; int S = 0;
    for (int l = 0; l < L; ++l) {
        for (int h = 0; h < H; ++h) {
            vec4 a = pos4[lo[l]];
            vec4 b = pos4[hi[h]];

            sect[S++] = (0 - a.w) / (b.w - a.w) * (b-a) + a;
        }
    }

    for(int s = 0; s < S; ++s) {
        emit(sect[s]);
    }

    EndPrimitive();
}
