#ifndef SIMPLEX_SOLIDS_H
#define SIMPLEX_SOLIDS_H

#include "mesh.h"
#include "rotor.h"

static auto T = (float) PI / 2;

Mesh<2> poly(int sides) {
    Mesh<2> res({}, {});

    auto t = (float) (PI * 2 / sides);
    float t0 = t / 2;
    auto r = 1 / cos(t0);

    for (int i = 0; i < sides; i++) {
        glm::vec2 p = r * glm::vec2(cos(t0 + t * i), sin(t0 + t * i));
        res.verts.emplace_back(p, 0, 0);
    }

    for (unsigned i = 0; i < sides - 1; i++) {
        res.inds.push_back(i);
        res.inds.push_back(i + 1);
    }

    return res;
}

Mesh<3> cube() {
    glm::vec4 off = glm::vec4(0, 0, 1, 0);
    Mesh<3> face = fill(poly(4));
    Mesh<3> pair = (face + off) + (face - off);

    return pair +
        rot_xz(T) * pair +
        rot_yz(T) * pair;
}

Mesh<4> tesseract() {
    glm::vec4 off = glm::vec4(0, 0, 0, 1);
    Mesh<4> cell = fill(cube());
    Mesh<4> pair = (cell + off) + (cell - off);

    return pair +
        rot_xw(T) * pair +
        rot_yw(T) * pair +
        rot_zw(T) * pair;
}

Mesh<4> tesseract_edge_frame(float width) {
    Mesh<4> edge = tesseract() * glm::vec4(width, width, width, 1);
    auto o = glm::vec3(1 - width);

    Mesh<4> set = (edge + glm::vec4(+o.x, +o.y, +o.z, 0)) +
        (edge + glm::vec4(+o.x, +o.y, -o.z, 0)) +
        (edge + glm::vec4(+o.x, -o.y, +o.z, 0)) +
        (edge + glm::vec4(+o.x, -o.y, -o.z, 0)) +
        (edge + glm::vec4(-o.x, +o.y, +o.z, 0)) +
        (edge + glm::vec4(-o.x, +o.y, -o.z, 0)) +
        (edge + glm::vec4(-o.x, -o.y, +o.z, 0)) +
        (edge + glm::vec4(-o.x, -o.y, -o.z, 0));

    return set +
        rot_xw(T) * set +
        rot_yw(T) * set +
        rot_zw(T) * set;
}

Mesh<4> tesseract_face_frame(float width) {
    //todo - not even sure what this would mean, but it should be possible.
    return Mesh<4>({}, {});
}

Mesh<4> tesseract_cell_frame(float width) {
    glm::vec4 off = glm::vec4(0, 0, 0, 1);
    Mesh<4> cell = join(cube() * (1 - width), cube());
    cell = cell + (cell - glm::vec4(0, 0, 0, width));
    Mesh<4> pair = (cell + off) + (cell + off) * -1.f;

    return pair +
        rot_xw(T) * pair +
        rot_yw(T) * pair +
        rot_zw(T) * pair;
}

#endif //SIMPLEX_SOLIDS_H
