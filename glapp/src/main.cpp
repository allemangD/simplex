#include <hello.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <cstdlib>
#include <cstdio>
#include <string>
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


static void error_callback(int error, const char *description) {
    fprintf(stderr, "Error: %s\n", description);
}


static void key_callback(GLFWwindow *window, int key, int scan_code, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}


int main() {
    hello();

    std::vector<Vertex> vertices = {
            {glm::vec2(+0.5f, +0.5f), glm::vec3(1, 0, 0)},
            {glm::vec2(+0.5f, -0.5f), glm::vec3(0, 1, 0)},
            {glm::vec2(-0.5f, -0.5f), glm::vec3(0, 0, 1)},
            {glm::vec2(-0.5f, +0.5f), glm::vec3(1, 1, 1)},
    };

    const int i_width = 1280, i_height = 720;

    GLFWwindow *window;
    GLFWmonitor *monitor;
    const GLFWvidmode *mode;
    GLuint vertex_buffer, vertex_shader, fragment_shader, program;
    GLuint pvm_location, vpos_location, vcol_location;

    glfwSetErrorCallback(error_callback);

    if (!glfwInit()) {
        exit(EXIT_FAILURE);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    window = glfwCreateWindow(i_width, i_height, "Hello Triangle", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    monitor = glfwGetPrimaryMonitor();
    mode = glfwGetVideoMode(monitor);
    glfwSetWindowPos(window, (mode->width - i_width) / 2, (mode->height - i_height) / 2);

    glfwSetKeyCallback(window, key_callback);
    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
    glfwSwapInterval(1);

    // NOTE: OpenGL error checks have been omitted for brevity
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

    while (!glfwWindowShouldClose(window)) {
        int width, height;
        float ratio;

        glfwGetFramebufferSize(window, &width, &height);
        ratio = (float) width / height;
        glViewport(0, 0, width, height);

        glClear(GL_COLOR_BUFFER_BIT);


        auto pvm = glm::ortho(-ratio, ratio, -1.f, 1.f);
        pvm = glm::rotate(pvm, (float) glfwGetTime(), glm::vec3(0.f, 0.f, 1.f));
        pvm = glm::scale(pvm, glm::vec3(0.9f));

        glUseProgram(program);
        glUniformMatrix4fv(pvm_location, 1, GL_FALSE, glm::value_ptr(pvm));

        glDrawArrays(GL_QUADS, 0, (GLsizei) vertices.size());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}
