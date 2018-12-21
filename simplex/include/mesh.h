#ifndef SIMPLEX_MESH_H
#define SIMPLEX_MESH_H

#include <algorithm>
#include <vector>

#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>

template<unsigned int prim>
struct Mesh {
    std::vector<glm::vec4> verts;
    std::vector<unsigned> inds;

    Mesh(std::vector<glm::vec4> verts, std::vector<unsigned> inds)
        : verts(std::move(verts)), inds(std::move(inds)) {}

    unsigned size() {
        return (unsigned) inds.size() / prim;
    }
};

template<unsigned int prim>
Mesh<prim> concat(Mesh<prim> m, Mesh<prim> n) {
    Mesh<prim> res({}, {});

    for (auto v: m.verts) res.verts.push_back(v);
    for (auto v: n.verts) res.verts.push_back(v);

    auto off = (unsigned) m.verts.size();

    for (auto i: m.inds) res.inds.push_back(i);
    for (auto i: n.inds) res.inds.push_back(off + i);

    return res;
}

template<unsigned int prim>
Mesh<prim> transform(Mesh<prim> m, glm::mat4 mat) {
    Mesh<prim> res(m.verts, m.inds);
    for (auto &vert : res.verts)
        vert = mat * vert;
    return res;
}

template<unsigned int prim>
Mesh<prim> offset(Mesh<prim> m, glm::vec4 off) {
    Mesh<prim> res(m.verts, m.inds);
    for (auto &vert : res.verts)
        vert += off;
    return res;
}

template<unsigned int prim>
Mesh<prim> scale(Mesh<prim> m, glm::vec4 scl) {
    Mesh<prim> res(m.verts, m.inds);
    for (auto &vert : res.verts)
        vert *= scl;
    return res;
}

template<unsigned int prim>
Mesh<prim> scale(Mesh<prim> m, float scl) {
    return scale(m, glm::vec4(scl));
}

template<unsigned int prim>
Mesh<prim + 1> pyramid(Mesh<prim> m, glm::vec4 apex) {
    Mesh<prim + 1> res(m.verts, {});
    res.verts.push_back(apex);
    auto apex_ind = (unsigned) res.verts.size() - 1;

    for (int i = 0; i < m.size(); ++i) {
        for (int j = 0; j < prim; ++j) {
            res.inds.push_back(m.inds[i * prim + j]);
        }
        res.inds.push_back(apex_ind);
    }

    return res;
}

template<unsigned int prim>
Mesh<prim + 1> fill(Mesh<prim> m) {
    Mesh<prim + 1> res(m.verts, {});

    for (int i = 0; i < m.size(); ++i) {
        for (int j = 0; j < prim; ++j) {
            res.inds.push_back(m.inds[i * prim + j]);
        }
        res.inds.push_back(0);
    }

    return res;
}

template<unsigned int prim>
Mesh<prim + 1> join(Mesh<prim> m, Mesh<prim> n) {
    Mesh<prim + 1> res({}, {});

    for (auto v : m.verts) res.verts.push_back(v);
    for (auto v : n.verts) res.verts.push_back(v);

    auto off = (unsigned) m.verts.size();
    auto size = (int) std::min(m.inds.size(), n.inds.size());

    for (int i = 0; i < size; i += prim) {
        for (int j = 0; j < prim; ++j) {
            for (int x = j; x < prim; ++x)
                res.inds.push_back(m.inds[i + x]);
            for (int x = 0; x <= j; ++x)
                res.inds.push_back(off + n.inds[i + x]);
        }
    }

    return res;
}

template<unsigned int prim>
Mesh<prim + 1> joinCap(Mesh<prim> m, Mesh<prim> n) {
    return concat(join(m, n), concat(fill(m), fill(n)));
}

Mesh<4> thicken(const Mesh<3> &m, float thickness) {
    return joinCap(offset(m, glm::vec4(0, 0, 0, -thickness / 2)), offset(m, glm::vec4(0, 0, 0, thickness / 2)));
}

template<unsigned int prim>
Mesh<prim> operator+(Mesh<prim> m, Mesh<prim> n) { return concat(m, n); }

template<unsigned int prim>
Mesh<prim> operator+(Mesh<prim> m, glm::vec4 off) { return offset(m, off); }

template<unsigned int prim>
Mesh<prim> operator-(Mesh<prim> m, glm::vec4 off) { return offset(m, -off); }

template<unsigned int prim>
Mesh<prim> operator*(Mesh<prim> m, glm::vec4 scl) { return scale(m, scl); }

template<unsigned int prim>
Mesh<prim> operator/(Mesh<prim> m, glm::vec4 scl) { return scale(m, 1.f / scl); }

template<unsigned int prim>
Mesh<prim> operator*(Mesh<prim> m, float scl) { return scale(m, scl); }

template<unsigned int prim>
Mesh<prim> operator/(Mesh<prim> m, float scl) { return scale(m, 1.f / scl); }

template<unsigned int prim>
Mesh<prim> operator*(glm::mat4 mat, Mesh<prim> m) { return transform(m, mat); }

#endif //SIMPLEX_MESH_H
