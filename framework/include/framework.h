#ifndef GL_TEMPLATE_FRAMEWORK_H
#define GL_TEMPLATE_FRAMEWORK_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <string>

class App {
private:
    GLFWwindow *_window = nullptr;
    int _gl_major, _gl_minor;
    std::string _title;
    float _last_time = 0, _time = 0, _rate = 1;
    float _last_glfw_time = 0, _glfw_time = 0;
    int _frame = 0;

    static void onKey(GLFWwindow *window, int key, int scan_code, int action, int mods);

    static void onSize(GLFWwindow *window, int width, int height);

    static void onCursorPos(GLFWwindow *window, double x, double y);

    static void onMouseButton(GLFWwindow *window, int button, int action, int mods);

protected:
    App(int gl_major, int gl_minor);

    void setTime(float time);

    void setFrame(int frame);

    void setRate(float rate);

    void setTitle(std::string title);

    void setX(int x);

    void setY(int y);

    void setPos(int x, int y);

    void setWidth(int width);

    void setHeight(int height);

    void setSize(int width, int height);

    void swapBuffers();

    virtual void onKey(int key, int scan_code, int action, int mods) {}

    virtual void onSize(int width, int height) {}

    virtual void onCursorPos(double x, double y) {}

    virtual void onMouseButton(int button, int action, int mods) {}

    virtual void init() {}

    virtual void display() {}

    virtual void update() {}

    virtual void deinit() {}

public:
    GLFWwindow *getWindow();

    int getFrame();

    float getRate();

    float getTime();

    float getTimeDelta();

    std::string getTitle();

    int getX();

    int getY();

    void getPos(int *x, int *y);

    int getWidth();

    int getHeight();

    void getSize(int *width, int *height);

    int run();

    void launch();

    void center();

    void center(GLFWmonitor *monitor);

    void close();
};

#endif //GL_TEMPLATE_FRAMEWORK_H
