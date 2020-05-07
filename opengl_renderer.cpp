#include <iostream>;
#include <string>;
#include <vector>;

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "opengl_renderer.h";
#include "shader.h";

float vertices[] = {
	// positions			// colors			// texture coords
	-0.9f, -0.5f, 0.0f,		1.0f, 0.0f, 0.0f,	0.0f, 0.0f,
	-0.5f, 0.5f, 0.0f,		0.0f, 1.0f, 0.0f,	0.5f, 1.0f,
	-0.1f, -0.5f, 0.0f,		0.0f, 0.0f, 1.0f,	1.0f, 0.0f
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

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

void OpenGLRenderer::run() {
	createWindow();
	setupShaders();
	createVertexBuffer();
	loadTexture();
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
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// aColor vertex attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// aTexCoord vertex attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	// Unbind the vertex buffer object
	//glBindBuffer(GL_ARRAY_BUFFER, 0);

	//// Second Triangle
	//glBindVertexArray(VAOs[1]);

	//glBindBuffer(GL_ARRAY_BUFFER, VBOs[1]);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(vertices2), vertices2, GL_STATIC_DRAW);

	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	//glEnableVertexAttribArray(0);

	//// Unbind the vertex buffer object
	//glBindBuffer(GL_ARRAY_BUFFER, 0);

	//// Unbind the Vertex Array Object (Generally not needed)
	//glBindVertexArray(0);
}

void OpenGLRenderer::setupShaders() {
	const std::string vertexPath = "D:\\Projects\\GitRepos\\voxel-engine\\opengl_vertex.glsl";
	const std::string fragmentPath = "D:\\Projects\\GitRepos\\voxel-engine\\opengl_fragment.glsl";
	const std::string fragment2Path = "D:\\Projects\\GitRepos\\voxel-engine\\opengl_fragment2.glsl";

	Shader triangle1Shader;
	triangle1Shader.load(vertexPath, fragmentPath);

	Shader triangle2Shader;
	triangle2Shader.load(vertexPath, fragment2Path);

	shaderPrograms[0] = triangle1Shader;
	shaderPrograms[1] = triangle2Shader;
}

void OpenGLRenderer::loadTexture() {

	stbi_set_flip_vertically_on_load(true);

	//unsigned int textureId;
	glGenTextures(1, &textureIds[0]);
	glBindTexture(GL_TEXTURE_2D, textureIds[0]);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	int width, height, channelCount;
	unsigned char* data = stbi_load("D:\\Projects\\GitRepos\\voxel-engine\\Assets\\textures\\wall.jpg", &width, &height, &channelCount, 0);

	if (!data) {
		stbi_image_free(data);

		throw std::runtime_error("Failed to generate texture");
	}

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);

	// free the memory where the image is loaded in.
	stbi_image_free(data);

	shaderPrograms[0].use();
	glUniform1i(glGetUniformLocation(shaderPrograms[0].programId, "textureData"), 0);

	glGenTextures(1, &textureIds[1]);
	glBindTexture(GL_TEXTURE_2D, textureIds[1]);

	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	//float borderColor[] = { 1.0f, 1.0f, 0.0f, 1.0f };
	//glTextureParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	data = stbi_load("D:\\Projects\\GitRepos\\voxel-engine\\Assets\\textures\\awesomeface.png", &width, &height, &channelCount, 0);

	if (!data) {
		stbi_image_free(data);

		throw std::runtime_error("Failed to generate texture");
	}

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);

	shaderPrograms[0].use();
	glUniform1i(glGetUniformLocation(shaderPrograms[0].programId, "textureData2"), 1);

	stbi_image_free(data);

}

void OpenGLRenderer::mainLoop() {

	std::cout << "start mainloop" << std::endl;

	while (!glfwWindowShouldClose(window)) {
		// TODO: Build input system

		// Clear screen
		glClearColor(0.2f, 0.3f, 0.3f, 1);
		glClear(GL_COLOR_BUFFER_BIT);

		// Set shader uniform horizontal offset value to interpolate to and from 0 to 1
		float time = glfwGetTime();
		float horizontalOffset = (sin(time) * 0.5f) + 0.5f;
		std::cout << horizontalOffset << std::endl;
		int ourColorUniformLocation = glGetUniformLocation(shaderPrograms[0].programId, "horizontalOffset");
		int opacityUniformLocation = glGetUniformLocation(shaderPrograms[0].programId, "opacity");

		// Bind texture so it will be assigned to the fragment shader's sampler
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureIds[0]);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, textureIds[1]);

		// Use the shader program before actually updating it
		shaderPrograms[0].use();
		glUniform1f(ourColorUniformLocation, horizontalOffset);
		glUniform1f(opacityUniformLocation, horizontalOffset);

		// Read from Vertex Array Object
		glBindVertexArray(VAOs[0]);
		// Draw first triangle
		glDrawArrays(GL_TRIANGLES, 0, 3);

		// Draw second triangle
		//shaderPrograms[1].use();
		//glBindVertexArray(VAOs[1]);
		//glDrawArrays(GL_TRIANGLES, 0, 3);

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
