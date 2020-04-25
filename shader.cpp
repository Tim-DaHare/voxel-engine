#include "shader.h";

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

void Shader::load(const std::string& vertexPath, const std::string& fragmentPath) {
	const std::string vertexShaderContent = fileContents(vertexPath);
	const char* vertexCharPointer = vertexShaderContent.c_str();

	const std::string fragmentShaderContent = fileContents(fragmentPath);
	const char* fragmentCharPointer = fragmentShaderContent.c_str();

	int success;
	char infoLog[512];

	unsigned int vertexShader;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexCharPointer, NULL);
	glCompileShader(vertexShader);

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

	programId = glCreateProgram();

	glAttachShader(programId, vertexShader);
	glAttachShader(programId, fragmentShader);
	glLinkProgram(programId);

	glGetProgramiv(programId, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(programId, 512, NULL, infoLog);
		throw std::runtime_error(infoLog);
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}

void Shader::use() {
	glUseProgram(programId);
}