#include "hello.h"

#include <GLFW/glfw3.h>

#include <stdio.h>
#include <string>

void hello() {
    printf("Hello, World!\n");
}

int showWindow(const std::string &title) {
    if (!glfwInit()) return EXIT_FAILURE;

    GLFWwindow *window = glfwCreateWindow(1280, 720, title.c_str(), nullptr, nullptr);

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return EXIT_SUCCESS;
}