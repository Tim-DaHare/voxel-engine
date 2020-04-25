#pragma once

#include <iostream>;
#include <fstream>;
#include <sstream>;

#include <glad/glad.h>

class Shader {

public:
	unsigned int programId;

	void load(const std::string& vertexPath, const std::string& fragmentPath);

	void use();

};