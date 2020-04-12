#include <iostream>

#include "opengl_renderer.h";

float vertices[] = {
	-0.9f, -0.5f, 0.0f,
	-0.5f, 0.5f, 0.0f,
	-0.1f, -0.5f, 0.0f,
};

float vertices2[] = {
	0.1f, -0.5f, 0.0f,
	0.5f, 0.5f, 0.0f,
	0.9f, -0.5f, 0.0f,
};

unsigned int indices[] = {  // note that we start from 0!
	0, 1, 2,
	//3, 4, 5
};

const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"}\0";

const char* fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
"}\0";

const char* fragmentShaderSource2 = "#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(1.0f, 1.0f, 0.0f, 1.0f);\n"
"}\0";

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

void OpenGLRenderer::run() {
	createWindow();
	setupShaders();
	createVertexBuffer();
	mainLoop();
	cleanup();
}

void OpenGLRenderer::createWindow() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	// Only needed for mac OS
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

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

void OpenGLRenderer::createVertexBuffer() {
	glGenVertexArrays(2, VAOs);
	glGenBuffers(2, VBOs);

	//First triangle
	glBindVertexArray(VAOs[0]);

	glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	//glGenBuffers(1, &ebo);

	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Tell OpenGL how to interpret the vertices and indices arrays as shader attributes
	// Since the VBO is still bound, this attribute will now bind to that VBO.
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// Unbind the vertex buffer object
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Second Triangle
	glBindVertexArray(VAOs[1]);

	glBindBuffer(GL_ARRAY_BUFFER, VBOs[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices2), vertices2, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// Unbind the vertex buffer object
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Unbind the Vertex Array Object (Generally not needed)
	glBindVertexArray(0);
}

void OpenGLRenderer::setupShaders() {
	unsigned int vertexShader;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);

	int success;

	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		char infoLog[512];

		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		throw std::runtime_error(infoLog);
	}

	unsigned int fragmentShader;
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);

	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		char infoLog[512];

		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		throw std::runtime_error(infoLog);
	}

	shaderPrograms[0] = glCreateProgram();

	glAttachShader(shaderPrograms[0], vertexShader);
	glAttachShader(shaderPrograms[0], fragmentShader);
	glLinkProgram(shaderPrograms[0]);

	glGetProgramiv(shaderPrograms[0], GL_LINK_STATUS, &success);
	if (!success) {
		char infoLog[512];
		glGetProgramInfoLog(shaderPrograms[0], 512, NULL, infoLog);

		throw std::runtime_error(infoLog);
	}

	//glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	// Yellow color shader
	shaderPrograms[1] = glCreateProgram();

	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource2, NULL);
	glCompileShader(fragmentShader);

	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		char infoLog[512];

		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		throw std::runtime_error(infoLog);
	}

	glAttachShader(shaderPrograms[1], vertexShader);
	glAttachShader(shaderPrograms[1], fragmentShader);
	glLinkProgram(shaderPrograms[1]);

	glGetProgramiv(shaderPrograms[1], GL_LINK_STATUS, &success);
	if (!success) {
		char infoLog[512];
		glGetProgramInfoLog(shaderPrograms[1], 512, NULL, infoLog);

		throw std::runtime_error(infoLog);
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}

void OpenGLRenderer::mainLoop() {

	while (!glfwWindowShouldClose(window)) {
		// TODO: Build input system

		// Clear screen
		glClearColor(0.2f, 0.3f, 0.3f, 1);
		glClear(GL_COLOR_BUFFER_BIT);

		// Read from shaderProgram and Vertex Array Object
		glUseProgram(shaderPrograms[0]);
		glBindVertexArray(VAOs[0]);
		glDrawArrays(GL_TRIANGLES, 0, 3);

		glUseProgram(shaderPrograms[1]);
		glBindVertexArray(VAOs[1]);
		glDrawArrays(GL_TRIANGLES, 0, 3);

		// Draw the data
		//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		
		glfwPollEvents();
		glfwSwapBuffers(window);
	}
}

void OpenGLRenderer::cleanup() {
	glDeleteVertexArrays(2, VAOs);
	glDeleteBuffers(2, VBOs);
	//glDeleteBuffers(1, &ebo);

	glfwTerminate();
}
