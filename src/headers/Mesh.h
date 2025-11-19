#pragma once

#include <glm/glm.hpp>
#include <string>
#include <vector>
#include "Shader.h"
#include <Camera.h>

struct Vertex {
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
	glm::vec3 Tangent;
	glm::vec3 Bitangent;
};

struct Texture {
	unsigned int id;
	std::string type; // difusse or specular
	std::string path;
};

class Mesh
{

public:
 // CAMBIAR ESTO A PRIVATE!!!!
	unsigned int VAO;

	std::vector<Vertex> vertices; 
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;

	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures) 
		: vertices(vertices), indices(indices), textures(textures)
	{
		setupMesh();
	}
	void Draw(Shader& shader);
	void InstancedDraw(Shader& shader, int amount);

private:
	unsigned int VBO, EBO;
	void setupMesh();

};

