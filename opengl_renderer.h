#pragma once

#include <glad/glad.h>;
#include <GLFW/glfw3.h>;

class OpenGLRenderer {

private:
    GLFWwindow* window;
    unsigned int vao;
    unsigned int shaderProgram;

    void createWindow();
    void createVertexBuffer();
    void setupShaders();
    void mainLoop();
    void cleanup();

public:
    void run();

};
