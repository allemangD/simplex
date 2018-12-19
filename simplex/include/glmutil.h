#ifndef GL_TEMPLATE_GLMUTILS_H
#define GL_TEMPLATE_GLMUTILS_H

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace glmutil {
    glm::mat4 rotation(float angle, glm::vec3 axis) {
        auto mat = glm::identity<glm::mat4>();
        return glm::rotate(mat, angle, axis);
    }

    glm::mat4 scale(glm::vec3 v) {
        auto mat = glm::identity<glm::mat4>();
        return glm::scale(mat, v);
    }

    glm::mat4 eulerAngles(glm::vec3 angles){
        auto mat = glm::identity<glm::mat4>();
        mat = glm::rotate(mat, angles.x, glm::vec3(1, 0, 0));
        mat = glm::rotate(mat, angles.y, glm::vec3(0, 1, 0));
        mat = glm::rotate(mat, angles.z, glm::vec3(0, 0, 1));
        return mat;
    }
}

#endif //GL_TEMPLATE_GLMUTILS_H
