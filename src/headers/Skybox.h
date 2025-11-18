#pragma once
#include <vector>
#include <string>
#include <glad/glad.h>
#include "stb_image.h"
#include <iostream>
#include "Shader.h"
#include "ShaderStorage.h"

class Skybox
{

public:
	Skybox(const std::vector<std::string>& faces);
	void Draw(const glm::mat4& projection, const glm::mat4& view);
	void Delete();

private:
	unsigned int VAO, VBO;
	unsigned int cubemapTexture;
	std::shared_ptr<Shader> shader;

	unsigned int loadCubemap(std::vector<std::string> faces);

};

/*
std::vector<std::string> faces{
	"textures/skybox/right.jpg",
	"textures/skybox/left.jpg",
	"textures/skybox/top.jpg",
	"textures/skybox/bottom.jpg",
	"textures/skybox/front.jpg",
	"textures/skybox/back.jpg"
};
*/