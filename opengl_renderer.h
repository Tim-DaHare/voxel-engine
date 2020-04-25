#pragma once

#include <glad/glad.h>;
#include <GLFW/glfw3.h>;

#include "shader.h";

class OpenGLRenderer {

private:
    GLFWwindow* window;
    unsigned int VAOs[2], VBOs[2], ebo;
    Shader shaderPrograms[2];

    void createWindow();
    void createVertexBuffer();
    void setupShaders();
    void mainLoop();
    void cleanup();

public:
    void run();

};
