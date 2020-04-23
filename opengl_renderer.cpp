#include <iostream>;
#include <string>;
#include <vector>;
#include <fstream>;
#include <sstream>;

#include "opengl_renderer.h";

float vertices[] = {
	// positions			// colors
	-0.9f, -0.5f, 0.0f,		1.0f, 0.0f, 0.0f,
	-0.5f, 0.5f, 0.0f,		0.0f, 1.0f, 0.0f,
	-0.1f, -0.5f, 0.0f,		0.0f, 0.0f, 1.0f,
};

float vertices2[] = {
	0.1f, -0.5f, 0.0f,
	0.5f, 0.5f, 0.0f,
	0.9f, -0.5f, 0.0f,
};

//unsigned int indices[] = {  // note that we start from 0!
//	0, 1, 2,
//	//3, 4, 5
//};

const std::string fileContents(const std::string& filename) {
	std::ifstream file(filename, std::ios::ate);

	if (!file.is_open()) {
		throw std::runtime_error("Failed to open file " + filename);
	}

	size_t fileSize = static_cast<size_t>(file.tellg());
	std::stringstream buffer;

	file.seekg(0);
	buffer << file.rdbuf();
	file.close();

	return buffer.str();
}

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

	std::cout << "Configure attr pointers" << std::endl;

	// Tell OpenGL how to interpret the vertices and indices arrays as shader attributes
	// Since the VBO is still bound, this attribute will now bind to that VBO.
	// aPos vertex attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// aColor vertex attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

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

	const std::string vertexShaderContent = fileContents("D:\\Projects\\GitRepos\\voxel-engine\\opengl_vertex.glsl");
	const char* vertexCharPointer = vertexShaderContent.c_str();

	const std::string fragmentShaderContent = fileContents("D:\\Projects\\GitRepos\\voxel-engine\\opengl_fragment.glsl");
	const char* fragmentCharPointer = fragmentShaderContent.c_str();

	const std::string fragment2ShaderContent = fileContents("D:\\Projects\\GitRepos\\voxel-engine\\opengl_fragment2.glsl");
	const char* fragment2CharPointer = fragment2ShaderContent.c_str();

	unsigned int vertexShader;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexCharPointer, NULL);
	glCompileShader(vertexShader);

	int success;
	char infoLog[512];

	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "Failed to compile vertex shader" << std::endl;
		throw std::runtime_error(infoLog);
	}

	unsigned int fragmentShader;
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentCharPointer, NULL);
	glCompileShader(fragmentShader);

	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		std::cout << "Failed to compile fragmentShader1" << std::endl;

		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		throw std::runtime_error(infoLog);
	}

	shaderPrograms[0] = glCreateProgram();

	glAttachShader(shaderPrograms[0], vertexShader);
	glAttachShader(shaderPrograms[0], fragmentShader);
	glLinkProgram(shaderPrograms[0]);

	glGetProgramiv(shaderPrograms[0], GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderPrograms[0], 512, NULL, infoLog);
		throw std::runtime_error(infoLog);
	}

	//glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	// Yellow color shader
	shaderPrograms[1] = glCreateProgram();

	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragment2CharPointer, NULL);
	glCompileShader(fragmentShader);

	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success) {

		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		throw std::runtime_error(infoLog);
	}

	glAttachShader(shaderPrograms[1], vertexShader);
	glAttachShader(shaderPrograms[1], fragmentShader);
	glLinkProgram(shaderPrograms[1]);

	glGetProgramiv(shaderPrograms[1], GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderPrograms[1], 512, NULL, infoLog);

		throw std::runtime_error(infoLog);
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}

void OpenGLRenderer::mainLoop() {

	std::cout << "start mainloop" << std::endl;

	while (!glfwWindowShouldClose(window)) {
		// TODO: Build input system

		// Clear screen
		glClearColor(0.2f, 0.3f, 0.3f, 1);
		glClear(GL_COLOR_BUFFER_BIT);

		// Set shader uniform color value to interpolate to and from 0 to 1
		//float time = glfwGetTime();
		//float greenValue = sin((time) * 0.5f) + 0.5f;
		//int ourColorUniformLocation = glGetUniformLocation(shaderPrograms[0], "ourColor");

		// Use the shader program before actually updating it
		glUseProgram(shaderPrograms[0]);

		//glUniform4f(ourColorUniformLocation, 0.5f, greenValue, 0, 1.0f);

		// Read from Vertex Array Object
		glBindVertexArray(VAOs[0]);
		// Draw first triangle
		glDrawArrays(GL_TRIANGLES, 0, 3);

		// Draw second triangle
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
