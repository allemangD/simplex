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

void push_tetrahedron(std::vector<unsigned> *inds, const std::vector<unsigned> &tetra) {
    for (int i = 0; i < 4; ++i) {
        inds->push_back(tetra[i]);
    }
}

void push_cube(std::vector<unsigned> *inds, const std::vector<unsigned> &cube) {
    push_tetrahedron(inds, {cube[0], cube[1], cube[2], cube[4]});
    push_tetrahedron(inds, {cube[1], cube[4], cube[5], cube[7]});
    push_tetrahedron(inds, {cube[1], cube[2], cube[3], cube[7]});
    push_tetrahedron(inds, {cube[2], cube[4], cube[6], cube[7]});
    push_tetrahedron(inds, {cube[1], cube[2], cube[4], cube[7]});
}

void push_tesseract(std::vector<unsigned> *inds, const std::vector<unsigned> &tess) {
    push_cube(inds, {tess[0], tess[1], tess[2], tess[3], tess[4], tess[5], tess[6], tess[7]});
    push_cube(inds, {tess[8], tess[9], tess[10], tess[11], tess[12], tess[13], tess[14], tess[15]});
    push_cube(inds, {tess[0], tess[1], tess[2], tess[3], tess[8], tess[9], tess[10], tess[11]});
    push_cube(inds, {tess[4], tess[5], tess[6], tess[7], tess[12], tess[13], tess[14], tess[15]});
    push_cube(inds, {tess[0], tess[1], tess[4], tess[5], tess[8], tess[9], tess[12], tess[13]});
    push_cube(inds, {tess[2], tess[3], tess[6], tess[7], tess[10], tess[11], tess[14], tess[15]});
    push_cube(inds, {tess[0], tess[2], tess[4], tess[6], tess[8], tess[10], tess[12], tess[14]});
    push_cube(inds, {tess[1], tess[3], tess[5], tess[7], tess[9], tess[11], tess[13], tess[15]});
}

void build_tesseract(std::vector<unsigned> *inds, std::vector<glm::vec4> *verts, const glm::vec4 center,
    const glm::vec4 size) {
    const glm::vec4 rad = size / 2.f;

    std::vector<glm::vec4> new_verts = {
        {center.x + size.x, center.y + size.y, center.z + size.z, center.w + size.w},
        {center.x + size.x, center.y + size.y, center.z + size.z, center.w - size.w},
        {center.x + size.x, center.y + size.y, center.z - size.z, center.w + size.w},
        {center.x + size.x, center.y + size.y, center.z - size.z, center.w - size.w},
        {center.x + size.x, center.y - size.y, center.z + size.z, center.w + size.w},
        {center.x + size.x, center.y - size.y, center.z + size.z, center.w - size.w},
        {center.x + size.x, center.y - size.y, center.z - size.z, center.w + size.w},
        {center.x + size.x, center.y - size.y, center.z - size.z, center.w - size.w},
        {center.x - size.x, center.y + size.y, center.z + size.z, center.w + size.w},
        {center.x - size.x, center.y + size.y, center.z + size.z, center.w - size.w},
        {center.x - size.x, center.y + size.y, center.z - size.z, center.w + size.w},
        {center.x - size.x, center.y + size.y, center.z - size.z, center.w - size.w},
        {center.x - size.x, center.y - size.y, center.z + size.z, center.w + size.w},
        {center.x - size.x, center.y - size.y, center.z + size.z, center.w - size.w},
        {center.x - size.x, center.y - size.y, center.z - size.z, center.w + size.w},
        {center.x - size.x, center.y - size.y, center.z - size.z, center.w - size.w},
    };

    std::vector<unsigned> new_inds;
    for (int i = 0; i < 16; i++) new_inds.push_back((unsigned) verts->size() + i);
    for (auto vert: new_verts) verts->push_back(vert);

    push_tesseract(inds, new_inds);
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

    bool DRAW_WIRE = true;

    void init() override {
        build_tesseract(&cell_elems, &cell_verts, {-1, -1, -1, +0}, {0.125, 0.125, 0.125, 1.125});
        build_tesseract(&cell_elems, &cell_verts, {-1, -1, +1, +0}, {0.125, 0.125, 0.125, 1.125});
        build_tesseract(&cell_elems, &cell_verts, {-1, +1, -1, +0}, {0.125, 0.125, 0.125, 1.125});
        build_tesseract(&cell_elems, &cell_verts, {-1, +1, +1, +0}, {0.125, 0.125, 0.125, 1.125});
        build_tesseract(&cell_elems, &cell_verts, {+1, -1, -1, +0}, {0.125, 0.125, 0.125, 1.125});
        build_tesseract(&cell_elems, &cell_verts, {+1, -1, +1, +0}, {0.125, 0.125, 0.125, 1.125});
        build_tesseract(&cell_elems, &cell_verts, {+1, +1, -1, +0}, {0.125, 0.125, 0.125, 1.125});
        build_tesseract(&cell_elems, &cell_verts, {+1, +1, +1, +0}, {0.125, 0.125, 0.125, 1.125});

        build_tesseract(&cell_elems, &cell_verts, {-1, -1, +0, -1}, {0.125, 0.125, 0.875, 0.125});
        build_tesseract(&cell_elems, &cell_verts, {-1, -1, +0, +1}, {0.125, 0.125, 0.875, 0.125});
        build_tesseract(&cell_elems, &cell_verts, {-1, +1, +0, -1}, {0.125, 0.125, 0.875, 0.125});
        build_tesseract(&cell_elems, &cell_verts, {-1, +1, +0, +1}, {0.125, 0.125, 0.875, 0.125});
        build_tesseract(&cell_elems, &cell_verts, {+1, -1, +0, -1}, {0.125, 0.125, 0.875, 0.125});
        build_tesseract(&cell_elems, &cell_verts, {+1, -1, +0, +1}, {0.125, 0.125, 0.875, 0.125});
        build_tesseract(&cell_elems, &cell_verts, {+1, +1, +0, -1}, {0.125, 0.125, 0.875, 0.125});
        build_tesseract(&cell_elems, &cell_verts, {+1, +1, +0, +1}, {0.125, 0.125, 0.875, 0.125});

        build_tesseract(&cell_elems, &cell_verts, {-1, +0, -1, -1}, {0.125, 0.875, 0.125, 0.125});
        build_tesseract(&cell_elems, &cell_verts, {-1, +0, -1, +1}, {0.125, 0.875, 0.125, 0.125});
        build_tesseract(&cell_elems, &cell_verts, {-1, +0, +1, -1}, {0.125, 0.875, 0.125, 0.125});
        build_tesseract(&cell_elems, &cell_verts, {-1, +0, +1, +1}, {0.125, 0.875, 0.125, 0.125});
        build_tesseract(&cell_elems, &cell_verts, {+1, +0, -1, -1}, {0.125, 0.875, 0.125, 0.125});
        build_tesseract(&cell_elems, &cell_verts, {+1, +0, -1, +1}, {0.125, 0.875, 0.125, 0.125});
        build_tesseract(&cell_elems, &cell_verts, {+1, +0, +1, -1}, {0.125, 0.875, 0.125, 0.125});
        build_tesseract(&cell_elems, &cell_verts, {+1, +0, +1, +1}, {0.125, 0.875, 0.125, 0.125});

        build_tesseract(&cell_elems, &cell_verts, {+0, -1, -1, -1}, {0.875, 0.125, 0.125, 0.125});
        build_tesseract(&cell_elems, &cell_verts, {+0, -1, -1, +1}, {0.875, 0.125, 0.125, 0.125});
        build_tesseract(&cell_elems, &cell_verts, {+0, -1, +1, -1}, {0.875, 0.125, 0.125, 0.125});
        build_tesseract(&cell_elems, &cell_verts, {+0, -1, +1, +1}, {0.875, 0.125, 0.125, 0.125});
        build_tesseract(&cell_elems, &cell_verts, {+0, +1, -1, -1}, {0.875, 0.125, 0.125, 0.125});
        build_tesseract(&cell_elems, &cell_verts, {+0, +1, -1, +1}, {0.875, 0.125, 0.125, 0.125});
        build_tesseract(&cell_elems, &cell_verts, {+0, +1, +1, -1}, {0.875, 0.125, 0.125, 0.125});
        build_tesseract(&cell_elems, &cell_verts, {+0, +1, +1, +1}, {0.875, 0.125, 0.125, 0.125});

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

        matrices.view = glm::lookAt(glm::vec3(0, 0, -4), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
        matrices.proj = glm::perspective(1.f, ratio, 0.1f, 20.0f);

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

        glUseProgram(sect_prog);
        glDrawArrays(GL_POINTS, 0, (GLsizei) cell_elems.size() / 4);

        if (DRAW_WIRE) {
            glUseProgram(wire_prog);
            glDrawArrays(GL_POINTS, 0, (GLsizei) cell_elems.size() / 4);
        }

        glBindVertexArray(0);

        glFinish();
        swapBuffers();
    }

    void onKey(int key, int scan_code, int action, int mods) override {
        if (action == GLFW_PRESS && key == GLFW_KEY_ESCAPE) {
            glfwSetWindowShouldClose(getWindow(), true);
        }

        if (action == GLFW_PRESS && key == GLFW_KEY_SPACE) {
            DRAW_WIRE = !DRAW_WIRE;
        }
    }

public:
    GLApp() : App(4, 4) {}
};


int main() {
    GLApp app = GLApp();
    app.launch();
}