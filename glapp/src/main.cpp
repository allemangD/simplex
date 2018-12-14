#include <framework.h>
#include <util.h>
#include "glmutil.h"

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/trigonometric.hpp>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <cstdlib>
#include <cstdio>
#include <vector>

struct Vertex {
    glm::vec2 pos;
    glm::vec3 col;
};

struct Matrices {
    glm::mat4 proj;
    glm::mat4 view;
    glm::mat4 model;
};

class GLApp : public App {
private:
    std::vector<Vertex> vertices;

    Matrices mats{
        glm::identity<glm::mat4>(),
        glm::identity<glm::mat4>(),
        glm::identity<glm::mat4>(),
    };

    glm::vec2 mouse_root = glm::vec2(0);
    glm::vec2 angle_root = glm::vec2(0);
    glm::vec2 angles = glm::vec2(0);

    GLuint circle_vert_arr = 0;
    GLuint circle_vert_buf = 0, matrix_buffer = 0;
    GLuint vertex_shader = 0, fragment_shader = 0, program = 0;
    GLuint vpos_location = 0, vcol_location = 0;
    GLuint matrices_index = 0;

    GLuint matrices_binding_point = 0;

protected:
    void onKey(int key, int scan_code, int action, int mods) override {
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
            close();
    }

    void onMouseButton(int button, int action, int mods) override {
        if (button == 0 && action == GLFW_PRESS) {
            double x, y;
            glfwGetCursorPos(getWindow(), &x, &y);
            mouse_root = glm::vec2(x, y);
            angle_root = angles;
        }
    }

    void onCursorPos(double x, double y)

    override {
        if (glfwGetMouseButton(getWindow(), 0)) {
            auto dx = (float) x - mouse_root.x;
            auto dy = (float) y - mouse_root.y;

            angles = angle_root + glm::vec2(dx, dy) / 100.f;
        }
    }

    void init() override {
        vertices.push_back({
            glm::vec2(0), glm::vec3(1)
        });

        const int N = 60;
        const float TAU = 6.28318f;

        for (int i = 0; i <= N; ++i) {
            float t = (float) i / N;

            auto p = glm::vec2(t) + glm::vec2(0, 1) / 4.f;
            auto c = glm::vec3(t) + glm::vec3(0, 1, 2) / 3.f;

            vertices.push_back({
                glm::cos(TAU * p),
                glm::cos(TAU * c)
            });
        }

        setTitle("Hello Square!");

        glGenVertexArrays(1, &circle_vert_arr);
        glBindVertexArray(circle_vert_arr);

        glGenBuffers(1, &circle_vert_buf);
        glBindBuffer(GL_ARRAY_BUFFER, circle_vert_buf);
        util::bufferData(GL_ARRAY_BUFFER, vertices, GL_STATIC_DRAW);

        program = util::buildProgram(false, {
            vertex_shader = util::buildShader("shaders/main.vert"),
            fragment_shader = util::buildShader("shaders/main.frag"),
        });

        glGenBuffers(1, &matrix_buffer);

        vpos_location = (GLuint) glGetAttribLocation(program, "vPos");
        vcol_location = (GLuint) glGetAttribLocation(program, "vCol");

        matrices_index = (GLuint) glGetUniformBlockIndex(program, "Matrices");
        glBindBufferBase(GL_UNIFORM_BUFFER, matrices_binding_point, matrix_buffer);
        glUniformBlockBinding(program, matrices_index, matrices_binding_point);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        glBindBuffer(GL_ARRAY_BUFFER, circle_vert_buf);
        glEnableVertexAttribArray(vpos_location);
        glVertexAttribPointer(vpos_location, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void *) nullptr);
        glEnableVertexAttribArray(vcol_location);
        glVertexAttribPointer(vcol_location, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void *) (sizeof(float) * 2));

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void display()

    override {
        int width, height;
        float ratio;

        glfwGetFramebufferSize(getWindow(), &width, &height);
        ratio = (float) width / height;
        glViewport(0, 0, width, height);

        glClear(GL_COLOR_BUFFER_BIT);

        mats.proj = glm::ortho(-ratio, ratio, -1.f, 1.f);
        mats.view = glmutil::scale(glm::vec3(0.9f)) * glmutil::eulerAngles(glm::vec3(angles.y, 0, angles.x));
        mats.model = glmutil::rotation(getTime(), glm::vec3(0.0f, 0.0f, 1.0f));

        glBindBuffer(GL_UNIFORM_BUFFER, matrix_buffer);
        glBufferData(GL_UNIFORM_BUFFER, sizeof(Matrices), &mats, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        glBindVertexArray(circle_vert_arr);
        glUseProgram(program);
        glDrawArrays(GL_TRIANGLE_FAN, 0, (GLsizei) vertices.size());
        glBindVertexArray(0);

        swapBuffers();
    }

    void deinit()

    override {
        glDeleteBuffers(1, &circle_vert_buf);
        glDeleteVertexArrays(1, &circle_vert_arr);
        glDeleteProgram(program);
        glDeleteShader(vertex_shader);
        glDeleteShader(fragment_shader);
    }

public:
    GLApp() : App(4, 4) {}
};


int main() {
    GLApp app = GLApp();
    app.launch();
}