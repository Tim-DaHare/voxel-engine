#include <iostream>
//#include <glad/glad.h>;
//#include <GLFW/glfw3.h>;

#include "opengl_renderer.h";

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

void OpenGLRenderer::run() {
	createWindow();
	mainLoop();
	cleanup();
}

void OpenGLRenderer::createWindow() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Only needed for mac OS
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	window = glfwCreateWindow(800, 600, "Voxel Engine", NULL, NULL);
	if (window == NULL) {
		cleanup();
		throw std::runtime_error("Failed to create OpenGL Window.");
	}

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		cleanup();
		throw std::runtime_error("Failed to initialize GLAD.");
	}

	glViewport(0, 0, 800, 600);
}

void OpenGLRenderer::mainLoop() {
	while (!glfwWindowShouldClose(window)) {
		// TODO: Build input system

		glClearColor(1, 0, 0, 1);
		glClear(GL_COLOR_BUFFER_BIT);
		
		glfwPollEvents();
		glfwSwapBuffers(window);
	}
}

void OpenGLRenderer::cleanup() {
	glfwTerminate();
}
