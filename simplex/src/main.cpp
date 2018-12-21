#include <unordered_map>
#include <vector>

#include <framework.h>
#include <gl_util.h>
#include <vsr/vsr.h>

#include "glmutil.h"
#include "mesh.h"
#include "rotor.h"
#include "solids.h"

extern "C" {
__attribute__((dllexport)) DWORD NvOptimusEnablement = 0x00000001;
}

struct Matrices {
    glm::mat4 model;
    glm::vec4 offset;

    glm::mat4 view;
    glm::mat4 proj;
};

class GLApp : public App {
    Mesh<4> mesh = Mesh<4>({}, {});

    Matrices matrices{};

    GLuint cell_array{};

    GLuint cell_vert_buf{}, cell_elem_arr_buf{}, matrix_buffer{};

    GLuint matrix_binding_point = 1;
    GLuint verts_binding_point = 1;

    GLuint wire_prog{}, sect_prog{};

    bool DRAW_WIRE = true;

    void init() override {
        mesh = tesseract();

        //region Uniforms
        matrices = {
            glm::identity<glm::mat4>(),
            glm::vec4(0),

            glm::identity<glm::mat4>(),
            glm::identity<glm::mat4>(),
        };
        //endregion

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
        util::bufferData(GL_SHADER_STORAGE_BUFFER, mesh.verts, GL_STATIC_DRAW);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

        glGenBuffers(1, &cell_elem_arr_buf);
        glBindBuffer(GL_ARRAY_BUFFER, cell_elem_arr_buf);
        util::bufferData(GL_ARRAY_BUFFER, mesh.inds, GL_STATIC_DRAW);
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

        matrices.model = glm::identity<glm::mat4>() *
            rotor(glm::vec4(1, 0, 0, 0), glm::vec4(0, 0, 0, 1), getTime() / 3) *
            rotor(glm::vec4(0, 1, 0, 0), glm::vec4(0, 0, 1, 0), getTime() / 3) *

//            rotor(glm::vec4(1, 1, 1, 0), glm::vec4(0, 0, 0, 1), getTime() / 3) *
//            rotor(glm::vec4(1, 0, 0, 0), glm::vec4(0, 0, 1, 0), getTime() / 3) *
            1.f;

//        matrices.offset = glm::vec4(0,0,0,sin(getTime() / 2));

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
        glDrawArrays(GL_POINTS, 0, mesh.size());

        if (DRAW_WIRE) {
            glClear(GL_DEPTH_BUFFER_BIT);
            glUseProgram(wire_prog);
            glDrawArrays(GL_POINTS, 0, mesh.size());
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