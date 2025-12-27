#pragma once
#include <vector>
#include <string>
#include <glad/glad.h>
#include "stb_image.h"
#include <iostream>
#include "Shader.h"
#include "ResourceStorage.h"

class Skybox
{

public:
	Skybox(const std::vector<std::string>& faces, const char* vertexPath, const char* fragmentPath);
	void Draw(const glm::mat4& projection, const glm::mat4& view);
	void Delete();

private:
	unsigned int VAO, VBO;
	unsigned int cubemapTexture;
	ResourceHandle shader;

	unsigned int loadCubemap(std::vector<std::string> faces);

};