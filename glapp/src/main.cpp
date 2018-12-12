#include <framework.h>
#include <util.h>

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

class GLApp : public App {
private:
    std::vector<Vertex> vertices;

    GLuint vertex_array = 0;
    GLuint vertex_buffer = 0;
    GLuint vertex_shader = 0, fragment_shader = 0, program = 0;
    GLuint pvm_location = 0, vpos_location = 0, vcol_location = 0;

protected:
    void onKey(int key, int scan_code, int action, int mods) override {
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
            close();
    }

    void init() override {
        vertices.push_back({
            glm::vec2(0), glm::vec3(1)
        });

        const int N = 16;
        const float TAU = 6.28318f;

        for (int i = 0; i <= N; ++i) {
            float t = (float) i / N;

            auto p = glm::vec2(t) + glm::vec2(0, 1) / 4.f;
            auto c = glm::vec3(t) + glm::vec3(0, 1, 2) / 3.f;

            vertices.push_back({
                glm::cos(TAU * p), glm::cos(TAU * c)
            });
        }

        setTitle("Hello Square!");

        glGenVertexArrays(1, &vertex_array);
        glBindVertexArray(vertex_array);

        glGenBuffers(1, &vertex_buffer);
        glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
        util::bufferData(GL_ARRAY_BUFFER, vertices, GL_STATIC_DRAW);

        program = util::buildProgram({
            vertex_shader = util::buildShader("shaders/main.vert"),
            fragment_shader = util::buildShader("shaders/main.frag"),
        });

        pvm_location = (GLuint) glGetUniformLocation(program, "pvm");
        vpos_location = (GLuint) glGetAttribLocation(program, "vPos");
        vcol_location = (GLuint) glGetAttribLocation(program, "vCol");

        glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
        glEnableVertexAttribArray(vpos_location);
        glVertexAttribPointer(vpos_location, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void *) nullptr);
        glEnableVertexAttribArray(vcol_location);
        glVertexAttribPointer(vcol_location, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void *) (sizeof(float) * 2));

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void display() override {
        int width, height;
        float ratio;

        glfwGetFramebufferSize(getWindow(), &width, &height);
        ratio = (float) width / height;
        glViewport(0, 0, width, height);

        glClear(GL_COLOR_BUFFER_BIT);

        auto pvm = glm::ortho(-ratio, ratio, -1.f, 1.f);
        pvm = glm::rotate(pvm, (float) glfwGetTime(), glm::vec3(0.f, 0.f, 1.f));
        pvm = glm::scale(pvm, glm::vec3(0.9f));

        glBindVertexArray(vertex_array);
        glUseProgram(program);
        glUniformMatrix4fv(pvm_location, 1, GL_FALSE, glm::value_ptr(pvm));
        glDrawArrays(GL_TRIANGLE_FAN, 0, (GLsizei) vertices.size());
        glBindVertexArray(0);

        swapBuffers();
    }

    void deinit() override {
        glDeleteBuffers(1, &vertex_buffer);
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