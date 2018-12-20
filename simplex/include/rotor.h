#ifndef GL_TEMPLATE_ROTOR_H
#define GL_TEMPLATE_ROTOR_H

#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vsr/vsr.h>

glm::mat4 rotor(glm::vec4 u, glm::vec4 v, float angle) {
    using Vec = vsr::euclidean_vector<4, float>;

    auto eu = *((Vec *) (void *) &u);
    auto ev = *((Vec *) (void *) &v);

    auto biv = (eu ^ ev).unit() * angle;

    Vec col[4] = {
        Vec(1, 0, 0, 0).rotate(biv),
        Vec(0, 1, 0, 0).rotate(biv),
        Vec(0, 0, 1, 0).rotate(biv),
        Vec(0, 0, 0, 1).rotate(biv)
    };

    return glm::make_mat4((float *) col);
}

#endif //GL_TEMPLATE_ROTOR_H
