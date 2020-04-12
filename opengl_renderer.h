#pragma once

#include <glad/glad.h>;
#include <GLFW/glfw3.h>;

class OpenGLRenderer {

private:
    GLFWwindow* window;
    unsigned int VAOs[2], VBOs[2], ebo;
    unsigned int shaderPrograms[2];

    void createWindow();
    void createVertexBuffer();
    void setupShaders();
    void mainLoop();
    void cleanup();

public:
    void run();

};
