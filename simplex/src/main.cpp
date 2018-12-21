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

void push_cube(std::vector<unsigned> &inds, std::vector<unsigned> cube) {
    std::vector<unsigned> cube_inds = {
        cube[0], cube[1], cube[2], cube[4],
        cube[1], cube[4], cube[5], cube[7],
        cube[1], cube[2], cube[3], cube[7],
        cube[2], cube[4], cube[6], cube[7],
        cube[1], cube[2], cube[4], cube[7],
    };

    for (auto ind : cube_inds) {
        inds.push_back(ind);
    }
}

class GLApp : public App {
    std::vector<glm::vec4> cell_verts{};
    std::vector<unsigned> cell_elems{};
    Matrices matrices{};

    GLuint cell_array{};

    GLuint cell_vert_buf{}, cell_elem_arr_buf{}, matrix_buffer{};

    GLuint matrix_binding_point = 1;
    GLuint verts_binding_point = 1;

    GLuint wire_prog{}, sect_prog{};

    void init() override {
        cell_verts = {
            glm::vec4(+1, +1, +1, +1) / 2.f,
            glm::vec4(+1, +1, +1, -1) / 2.f,
            glm::vec4(+1, +1, -1, +1) / 2.f,
            glm::vec4(+1, +1, -1, -1) / 2.f,
            glm::vec4(+1, -1, +1, +1) / 2.f,
            glm::vec4(+1, -1, +1, -1) / 2.f,
            glm::vec4(+1, -1, -1, +1) / 2.f,
            glm::vec4(+1, -1, -1, -1) / 2.f,
            glm::vec4(-1, +1, +1, +1) / 2.f,
            glm::vec4(-1, +1, +1, -1) / 2.f,
            glm::vec4(-1, +1, -1, +1) / 2.f,
            glm::vec4(-1, +1, -1, -1) / 2.f,
            glm::vec4(-1, -1, +1, +1) / 2.f,
            glm::vec4(-1, -1, +1, -1) / 2.f,
            glm::vec4(-1, -1, -1, +1) / 2.f,
            glm::vec4(-1, -1, -1, -1) / 2.f,
        };

        cell_elems = {};
        push_cube(cell_elems, {0, 1, 2, 3, 4, 5, 6, 7});
        push_cube(cell_elems, {8, 9, 10, 11, 12, 13, 14, 15});
        push_cube(cell_elems, {0, 1, 2, 3, 8, 9, 10, 11});
        push_cube(cell_elems, {4, 5, 6, 7, 12, 13, 14, 15});
        push_cube(cell_elems, {0, 1, 4, 5, 8, 9, 12, 13});
        push_cube(cell_elems, {2, 3, 6, 7, 10, 11, 14, 15});
        push_cube(cell_elems, {0, 2, 4, 6, 8, 10, 12, 14});
        push_cube(cell_elems, {1, 3, 5, 7, 9, 11, 13, 15});

        matrices = {
            glm::identity<glm::mat4>(),
            glm::identity<glm::mat4>(),
            glm::identity<glm::mat4>(),
        };

        //region Shaders
        GLuint main_vs = util::buildShader(GL_VERTEX_SHADER, {"shaders/main.vert"});
        GLuint sect_fs = util::buildShader(GL_FRAGMENT_SHADER, {"shaders/sect.frag"});
        GLuint wire_fs = util::buildShader(GL_FRAGMENT_SHADER, {"shaders/wire.frag"});
        GLuint sect_gs = util::buildShader(GL_GEOMETRY_SHADER, {"shaders/sect.geom"});
        GLuint wire_gs = util::buildShader(GL_GEOMETRY_SHADER, {"shaders/wire.geom"});

        wire_prog = util::buildProgram(false, {main_vs, wire_fs, wire_gs});
        sect_prog = util::buildProgram(false, {main_vs, sect_fs, sect_gs});

        glDeleteShader(main_vs);
        glDeleteShader(sect_fs);
        glDeleteShader(wire_fs);
        glDeleteShader(sect_gs);
        glDeleteShader(wire_gs);
        //endregion

        //region Buffers
        glGenBuffers(1, &cell_vert_buf);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, verts_binding_point, cell_vert_buf);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, cell_vert_buf);
        util::bufferData(GL_SHADER_STORAGE_BUFFER, cell_verts, GL_STATIC_DRAW);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

        glGenBuffers(1, &cell_elem_arr_buf);
        glBindBuffer(GL_ARRAY_BUFFER, cell_elem_arr_buf);
        util::bufferData(GL_ARRAY_BUFFER, cell_elems, GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glGenBuffers(1, &matrix_buffer);
        glBindBufferBase(GL_UNIFORM_BUFFER, matrix_binding_point, matrix_buffer);
        glBindBuffer(GL_UNIFORM_BUFFER, matrix_buffer);
        util::bufferData(GL_UNIFORM_BUFFER, matrices, GL_STREAM_DRAW);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
        //endregion

        //region Vertex Arrays
        glGenVertexArrays(1, &cell_array);
        glBindVertexArray(cell_array);

        glBindBuffer(GL_ARRAY_BUFFER, cell_elem_arr_buf);
        auto ind_loc = (GLuint) glGetAttribLocation(wire_prog, "vInds");
        glEnableVertexAttribArray(ind_loc);
        glVertexAttribIPointer(ind_loc, 4, GL_UNSIGNED_INT, sizeof(int) * 4, (void *) nullptr);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glBindVertexArray(0);
        //endregion
    };

    void update() override {
        int width, height;
        glfwGetFramebufferSize(getWindow(), &width, &height);
        float ratio = (float) width / height;

        matrices.model =
            rotor(glm::vec4(0, 1, 0, 0), glm::vec4(0, 0, 1, 0), getTime() / 4) *
                rotor(glm::vec4(1, 1, 1, 0), glm::vec4(0, 0, 0, 1), getTime() / 5);

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

        glEnable(GL_DEPTH_TEST);

        glBindVertexArray(cell_array);

        glUseProgram(wire_prog);
        glDrawArrays(GL_POINTS, 0, (GLsizei) cell_elems.size() / 4);

        glClear(GL_DEPTH_BUFFER_BIT);
        glUseProgram(sect_prog);
        glDrawArrays(GL_POINTS, 0, (GLsizei) cell_elems.size() / 4);

        glUseProgram(wire_prog);
        glDrawArrays(GL_POINTS, 0, (GLsizei) cell_elems.size() / 4);

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