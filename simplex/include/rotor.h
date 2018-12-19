#ifndef GL_TEMPLATE_ROTOR_H
#define GL_TEMPLATE_ROTOR_H

#include <glm/mat4x4.hpp>

#include <math.h>

glm::mat4 rotor_xy(float t) {
    float c = cos(t);
    float s = sin(t);

    return glm::mat4(
        +c, s, 0, 0,
        -s, c, 0, 0,
        +0, 0, 1, 0,
        +0, 0, 0, 1
    );
}

glm::mat4 rotor_xz(float t) {
    float c = cos(t);
    float s = sin(t);

    return glm::mat4(
        +c, 0, s, 0,
        +0, 1, 0, 0,
        -s, 0, c, 0,
        +0, 0, 0, 1
    );
}

glm::mat4 rotor_xw(float t) {
    float c = cos(t);
    float s = sin(t);

    return glm::mat4(
        +c, 0, 0, s,
        +0, 1, 0, 0,
        +0, 0, 1, 0,
        -s, 0, 0, c
    );
}

glm::mat4 rotor_yz(float t) {
    float c = cos(t);
    float s = sin(t);

    return glm::mat4(
        1, +0, 0, 0,
        0, +c, s, 0,
        0, -s, c, 0,
        0, +0, 0, 1
    );
}

glm::mat4 rotor_yw(float t) {
    float c = cos(t);
    float s = sin(t);

    return glm::mat4(
        1, +0, 0, 0,
        0, +c, 0, s,
        0, +0, 1, 0,
        0, -s, 0, c
    );
}

glm::mat4 rotor_zw(float t) {
    float c = cos(t);
    float s = sin(t);

    return glm::mat4(
        1, 0, +0, 0,
        0, 1, +0, 0,
        0, 0, +c, s,
        0, 0, -s, c
    );
}

#endif //GL_TEMPLATE_ROTOR_H
