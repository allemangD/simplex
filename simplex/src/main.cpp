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

#include <iostream>
#include <glm/ext.hpp>

#include <cstdlib>
#include <cstdio>
#include <vector>

#define PI 3.14159f

struct Matrices {
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 proj;
};

void push_point(
    std::vector<glm::vec4> &verts, std::vector<unsigned> &elems,
    float xi_1, float xi_2, float eta) {

    float x = cos((xi_2 + xi_1) / 2) * cos(eta);
    float y = sin((xi_2 + xi_1) / 2) * cos(eta);
    float z = cos((xi_2 - xi_1) / 2) * sin(eta);
    float w = sin((xi_2 - xi_1) / 2) * sin(eta);

    verts.emplace_back(x / (1 - w), y / (1 - w), z / (1 - w), w); // stereographic

//    verts.emplace_back(x, y, z, w); // orthographic
}

void push_circle(
    std::vector<glm::vec4> &verts, std::vector<unsigned> &elems,
    float xi_1, float eta) {

    const auto N = (unsigned) 128;
    const auto R = (unsigned) verts.size();

    for (unsigned k = 0; k < N; ++k) {
        float xi_2 = 4 * PI * k / N;
        push_point(verts, elems, xi_1, xi_2, eta);

        elems.push_back(R + (k) % N);
        elems.push_back(R + (k + 1) % N);
    }
}

class GLApp : public App {
    std::vector<glm::vec4> cell_verts{};
    std::vector<unsigned> cell_elems{};
    Matrices matrices{};

    GLuint cell_array{};
    GLuint cell_vert_buf{}, cell_elem_arr_buf{};
    GLuint wire_prog{};

    GLuint matrix_buffer{};
    GLuint matrix_binding_point = 1;

    void generate() {
        cell_verts.clear();
        cell_elems.clear();

        // xi_1 from 0 to 2pi
        // xi_2 from 0 to 4pi
        // eta from 0 to pi/2

        const unsigned M = 2;
        const unsigned N = 32;
        const float PAD = 0.025;

        for (unsigned j = 0; j <= M; ++j) {
            float eta = (PI / 2 - PAD * 2) * (1 + cos(getTime() / 6)) / 2 * j / M + PAD;

            for (unsigned k = 0; k < N; ++k) {
                float xi_1 = PI * 2 * k / N;

                push_circle(cell_verts, cell_elems, xi_1, eta);
            }
        }

        const unsigned O = 256;

        for (unsigned k = 0; k < O; ++k) {
            float t = (float) k / O;
            int loops = 10;
            float xi_1 = PI * 2 * (t + cos(loops * PI * t + getTime() / 6) / loops);
            float eta =
                sin(loops * PI * t) / loops + .5f + (cos(xi_1 + getTime() * 2) + cos(xi_1 + getTime() * 1.6f)) / 12;

            push_circle(cell_verts, cell_elems, xi_1, eta);
        }


        glBindBuffer(GL_ARRAY_BUFFER, cell_vert_buf);
        util::bufferData(GL_ARRAY_BUFFER, cell_verts, GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cell_elem_arr_buf);
        util::bufferData(GL_ELEMENT_ARRAY_BUFFER, cell_elems, GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    void init() override {
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

        glGenBuffers(1, &cell_elem_arr_buf);

        glGenBuffers(1, &matrix_buffer);
        glBindBufferBase(GL_UNIFORM_BUFFER, matrix_binding_point, matrix_buffer);
        glBindBuffer(GL_UNIFORM_BUFFER, matrix_buffer);
        util::bufferData(GL_UNIFORM_BUFFER, matrices, GL_STREAM_DRAW);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
        //endregion

        generate();

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

//        matrices.model = rotor_yw(getTime() / 5) * rotor_xz(getTime() / 2);
//
        float t = getTime() / 5;
//        matrices.view = glm::lookAt(glm::vec3(0, 0, 0), -glm::vec3(cos(t), sin(t), .25), glm::vec3(0, 0, 1));

//        matrices.proj = glm::perspective(1.f, ratio, 0.1f, 100.0f);
        float W = 2;
        float D = 10;
        matrices.proj = glm::ortho(-ratio * W, ratio * W, -W, W, -D, D);

        glBindBuffer(GL_UNIFORM_BUFFER, matrix_buffer);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(Matrices), &matrices);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        generate();
    }

    void display() override {
        int width, height;
        glfwGetFramebufferSize(getWindow(), &width, &height);

        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glPointSize(10);
        glLineWidth(5);

        glEnable(GL_DEPTH_TEST);
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

    glm::vec3 euler{};
    double x_ = 0, y_ = 0;

    void onCursorPos(double x, double y) override {
        auto dx = (float) (x - x_) / 100;
        auto dy = (float) (y - y_) / 100;

        if (glfwGetMouseButton(getWindow(), 0)) {
            euler += glm::vec3(dy, 0, dx);

            matrices.view = glm::identity<glm::mat4>();
            matrices.view = glm::rotate(matrices.view, euler.x, glm::vec3(1, 0, 0));
            matrices.view = glm::rotate(matrices.view, euler.y, glm::vec3(0, 1, 0));
            matrices.view = glm::rotate(matrices.view, euler.z, glm::vec3(0, 0, 1));
        }

        x_ = x;
        y_ = y;
    }

public:
    GLApp() : App(4, 4) {}
};


int main() {
    GLApp app = GLApp();
    app.launch();
}