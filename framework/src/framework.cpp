#include "framework.h"

#include <unordered_map>

App::App(int gl_major, int gl_minor) {
    _gl_major = gl_major;
    _gl_minor = gl_minor;
}

void App::setTitle(std::string title) {
    _title = title;
    glfwSetWindowTitle(getWindow(), title.c_str());
}

void App::setX(int x) {
    setPos(x, getY());
}

void App::setY(int y) {
    setPos(getX(), y);
}

void App::setPos(int x, int y) {
    glfwSetWindowPos(getWindow(), x, y);
}

void App::setWidth(int width) {
    setSize(width, getHeight());
}

void App::setHeight(int height) {
    setSize(getWidth(), height);
}

void App::setSize(int width, int height) {
    glfwSetWindowSize(getWindow(), width, height);
}

void App::swapBuffers() {
    glfwSwapBuffers(getWindow());
}

GLFWwindow *App::getWindow() {
    return _window;
}

std::string App::getTitle() {
    return _title;
}

int App::getX() {
    int x;
    getPos(&x, nullptr);
    return x;
}

int App::getY() {
    int y;
    getPos(nullptr, &y);
    return y;
}

void App::getPos(int *x, int *y) {
    glfwGetWindowPos(getWindow(), x, y);
}

int App::getWidth() {
    int w;
    getSize(&w, nullptr);
    return w;
}

int App::getHeight() {
    int h;
    getSize(nullptr, &h);
    return h;
}

void App::getSize(int *width, int *height) {
    glfwGetWindowSize(getWindow(), width, height);
}

void App::center() {
    center(glfwGetPrimaryMonitor());
}

void App::center(GLFWmonitor *monitor) {
    int x, y;
    int w, h;
    const GLFWvidmode *mode = glfwGetVideoMode(monitor);

    glfwGetMonitorPos(monitor, &x, &y);
    getSize(&w, &h);

    setPos(x + (mode->width - w) / 2,
        y + (mode->height - h) / 2);
}

void App::close() {
    glfwSetWindowShouldClose(getWindow(), GLFW_TRUE);
}

std::unordered_map<GLFWwindow *, App *> manager;

App *get(GLFWwindow *window) {
    auto m = manager.find(window);
    if (m == manager.end())
        return nullptr;
    return m->second;
}

void App::onKey(GLFWwindow *window, int key, int scan_code, int action, int mods) {
    auto *app = get(window);
    if (app) app->onKey(key, scan_code, action, mods);
}

void App::onSize(GLFWwindow *window, int width, int height) {
    auto *app = get(window);
    if (app) app->onSize(width, height);
}

void App::onCursorPos(GLFWwindow *window, double x, double y) {
    auto *app = get(window);
    if (app) app->onCursorPos(x, y);
}

void App::onMouseButton(GLFWwindow *window, int button, int action, int mods) {
    auto *app = get(window);
    if (app) app->onMouseButton(button, action, mods);
}

int App::run() {
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, _gl_major);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, _gl_minor);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    _title = "GLFW App";

    if (!(_window = glfwCreateWindow(1280, 720, _title.c_str(), nullptr, nullptr)))
        return EXIT_FAILURE;

    center();

    glfwSetWindowSizeCallback(getWindow(), App::onSize);
    glfwSetKeyCallback(getWindow(), App::onKey);
    glfwSetCursorPosCallback(getWindow(), App::onCursorPos);
    glfwSetMouseButtonCallback(getWindow(), App::onMouseButton);

    glfwMakeContextCurrent(_window);
    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
    glfwSwapInterval(0);

    init();

    manager[getWindow()] = this;
    while (!glfwWindowShouldClose(_window)) {
        display();

        glfwPollEvents();
    };
    manager.erase(getWindow());

    deinit();

    glfwDestroyWindow(_window);

    return EXIT_SUCCESS;
}

void error_callback(int error, const char *description) {
    fprintf(stderr, "Error (%d): %s\n", error, description);
}

void App::launch() {
    if (!glfwInit()) exit(EXIT_FAILURE);

    glfwSetErrorCallback(error_callback);

    int code = run();

    glfwTerminate();

    exit(code);
}
