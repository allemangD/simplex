#include <framework.h>

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <cstdlib>
#include <cstdio>
#include <vector>

struct Vertex {
    glm::vec2 pos;
    glm::vec3 col;
};

static const char *vertex_shader_text =
        "uniform mat4 pvm;\n"
        "attribute vec3 vCol;\n"
        "attribute vec2 vPos;\n"
        "varying vec3 color;\n"
        "void main()\n"
        "{\n"
        "    gl_Position = pvm * vec4(vPos, 0.0, 1.0);\n"
        "    color = vCol;\n"
        "}\n";


static const char *fragment_shader_text =
        "varying vec3 color;\n"
        "void main()\n"
        "{\n"
        "    gl_FragColor = vec4(color, 1.0);\n"
        "}\n";


class GLApp : public App {
private:
    std::vector<Vertex> vertices = {
            {glm::vec2(+0.5f, +0.5f), glm::vec3(1, 0, 0)},
            {glm::vec2(+0.5f, -0.5f), glm::vec3(0, 1, 0)},
            {glm::vec2(-0.5f, -0.5f), glm::vec3(0, 0, 1)},
            {glm::vec2(-0.5f, +0.5f), glm::vec3(1, 1, 1)},
    };

    GLuint vertex_buffer = 0;
    GLuint vertex_shader = 0, fragment_shader = 0, program = 0;
    GLuint pvm_location = 0, vpos_location = 0, vcol_location = 0;

protected:
    void onKey(int key, int scan_code, int action, int mods) override {
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
            close();
    }

    void init() override {
        setTitle("Hello Square!");

        glGenBuffers(1, &vertex_buffer);
        glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices.front(), GL_STATIC_DRAW);

        vertex_shader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex_shader, 1, &vertex_shader_text, nullptr);
        glCompileShader(vertex_shader);

        fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment_shader, 1, &fragment_shader_text, nullptr);
        glCompileShader(fragment_shader);

        program = glCreateProgram();
        glAttachShader(program, vertex_shader);
        glAttachShader(program, fragment_shader);
        glLinkProgram(program);

        pvm_location = (GLuint) glGetUniformLocation(program, "pvm");
        vpos_location = (GLuint) glGetAttribLocation(program, "vPos");
        vcol_location = (GLuint) glGetAttribLocation(program, "vCol");

        glEnableVertexAttribArray(vpos_location);
        glVertexAttribPointer(vpos_location, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void *) nullptr);

        glEnableVertexAttribArray(vcol_location);
        glVertexAttribPointer(vcol_location, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void *) (sizeof(float) * 2));
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

        glUseProgram(program);
        glUniformMatrix4fv(pvm_location, 1, GL_FALSE, glm::value_ptr(pvm));

        glDrawArrays(GL_QUADS, 0, (GLsizei) vertices.size());

        swapBuffers();
    }

public:
    GLApp() : App(3, 0) {}
};


int main() {
    GLApp app = GLApp();
    app.launch();
}