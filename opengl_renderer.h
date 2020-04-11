#pragma once

#include <glad/glad.h>;
#include <GLFW/glfw3.h>;

class OpenGLRenderer {

private:
    GLFWwindow* window;

    void createWindow();
    void mainLoop();
    void cleanup();

public:
    void run();
};
