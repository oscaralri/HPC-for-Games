#pragma once

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <glad/glad.h>

class Shader
{
private:
	const char* vShaderCode;
	const char* fShaderCode;

	void readFile(const char* vertexPath, const char* fragmentPath);
	void compile();
	void checkCompileErrors(unsigned int shader, std::string type);

public:
	unsigned int ID; 
	Shader(const char* vertexPath, const char* fragmentPath);
	void use();
	void setBool(const std::string &name, bool value) const;
	void setInt(const std::string &name, int value) const;
	void setFloat(const std::string &name, float value) const;
};

