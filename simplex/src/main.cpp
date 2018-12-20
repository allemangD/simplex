#include <framework.h>
#include <util.h>

#include "glmutil.h"
#include "rotor.h"

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/trigonometric.hpp>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <cstdlib>
#include <cstdio>
#include <vector>

struct Matrices {
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 proj;
};

class GLApp : public App {
    std::vector<glm::vec4> cell_verts{};
    std::vector<unsigned> cell_elems{};
    Matrices matrices{};

    GLuint cell_array{};
    GLuint cell_vert_buf{}, cell_elem_arr_buf{};
    GLuint wire_prog{};

    GLuint matrix_buffer{};
    GLuint matrix_binding_point = 1;

    void init() override {
        cell_verts = {
            glm::vec4(-0.5, -0.5, -0.5, -0.5),
            glm::vec4(-0.5, -0.5, -0.5, +0.5),
            glm::vec4(-0.5, -0.5, +0.5, -0.5),
            glm::vec4(-0.5, -0.5, +0.5, +0.5),
            glm::vec4(-0.5, +0.5, -0.5, -0.5),
            glm::vec4(-0.5, +0.5, -0.5, +0.5),
            glm::vec4(-0.5, +0.5, +0.5, -0.5),
            glm::vec4(-0.5, +0.5, +0.5, +0.5),
            glm::vec4(+0.5, -0.5, -0.5, -0.5),
            glm::vec4(+0.5, -0.5, -0.5, +0.5),
            glm::vec4(+0.5, -0.5, +0.5, -0.5),
            glm::vec4(+0.5, -0.5, +0.5, +0.5),
            glm::vec4(+0.5, +0.5, -0.5, -0.5),
            glm::vec4(+0.5, +0.5, -0.5, +0.5),
            glm::vec4(+0.5, +0.5, +0.5, -0.5),
            glm::vec4(+0.5, +0.5, +0.5, +0.5),
        };

        cell_elems = {
            0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
            0, 2, 4, 6, 8, 10, 12, 14, 1, 3, 5, 7, 9, 11, 13, 15,
            0, 4, 8, 12, 1, 5, 9, 13, 2, 6, 10, 14, 3, 7, 11, 15,
            0, 8, 1, 9, 2, 10, 3, 11, 4, 12, 5, 13, 6, 14, 7, 15,
        };

        matrices = {
            glm::identity<glm::mat4>(),
            glm::identity<glm::mat4>(),
            glm::identity<glm::mat4>(),
        };

        //region Shaders
        GLuint vs = util::buildShader(GL_VERTEX_SHADER, {"shaders/main.vert"});
        GLuint fs = util::buildShader(GL_FRAGMENT_SHADER, {"shaders/main.frag"});
        wire_prog = util::buildProgram(false, {vs, fs});
        glDeleteShader(vs);
        glDeleteShader(fs);
        //endregion

        //region Buffers
        glGenBuffers(1, &cell_vert_buf);
        glBindBuffer(GL_ARRAY_BUFFER, cell_vert_buf);
        util::bufferData(GL_ARRAY_BUFFER, cell_verts, GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glGenBuffers(1, &cell_elem_arr_buf);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cell_elem_arr_buf);
        util::bufferData(GL_ELEMENT_ARRAY_BUFFER, cell_elems, GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        glGenBuffers(1, &matrix_buffer);
        glBindBufferBase(GL_UNIFORM_BUFFER, matrix_binding_point, matrix_buffer);
        glBindBuffer(GL_UNIFORM_BUFFER, matrix_buffer);
        util::bufferData(GL_UNIFORM_BUFFER, matrices, GL_STREAM_DRAW);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
        //endregion

        //region Vertex Arrays
        glGenVertexArrays(1, &cell_array);
        glBindVertexArray(cell_array);

        glBindBuffer(GL_ARRAY_BUFFER, cell_vert_buf);
        auto pos4_loc = (GLuint) glGetAttribLocation(wire_prog, "pos4");
        glEnableVertexAttribArray(pos4_loc);
        glVertexAttribPointer(pos4_loc, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), (void *) nullptr);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cell_elem_arr_buf);
        glBindVertexArray(0);
        //endregion
    };

    void update() override {
        int width, height;
        glfwGetFramebufferSize(getWindow(), &width, &height);
        float ratio = (float) width / height;

        matrices.model =
            rotor(glm::vec4(1,0,0,0), glm::vec4(0,0,1,0), getTime() / 4) *
            rotor(glm::vec4(0,1,0,0), glm::vec4(0,0,0,1), getTime() / 4);

        matrices.view = glm::lookAt(glm::vec3(0, 0, -2), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
        matrices.proj = glm::perspective(1.f, ratio, 0.1f, 10.0f);

        glBindBuffer(GL_UNIFORM_BUFFER, matrix_buffer);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(Matrices), &matrices);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }

    void display() override {
        int width, height;
        glfwGetFramebufferSize(getWindow(), &width, &height);

        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glPointSize(10);
        glLineWidth(2);

        glEnable(GL_DEPTH_BUFFER_BIT);
        glUseProgram(wire_prog);
        glBindVertexArray(cell_array);
        glDrawElements(GL_LINES, (GLsizei) cell_elems.size(), GL_UNSIGNED_INT, nullptr);
//        glDrawArrays(GL_POINTS, 0, (GLsizei) cell_verts.size());
        glBindVertexArray(0);

        glFinish();
        swapBuffers();
    }

    void onKey(int key, int scan_code, int action, int mods) override {
        if (action == GLFW_PRESS && key == GLFW_KEY_ESCAPE) {
            glfwSetWindowShouldClose(getWindow(), true);
        }
    }

public:
    GLApp() : App(4, 4) {}
};


int main() {
    GLApp app = GLApp();
    app.launch();
}