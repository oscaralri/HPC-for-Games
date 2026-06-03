#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <glad/glad.h>
#include <string>
#include "ResourceStorage.h"
#include "Shader.h"

struct Vertex {
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
	glm::vec3 Tangent;
	glm::vec3 Bitangent;

	// batching
	float TexIndex;
};

class Mesh
{
public:
 // CAMBIAR ESTO A PRIVATE!!!!
	unsigned int VAO;

	std::vector<Vertex> vertices; 
	std::vector<unsigned int> indices;
	float texIndex;

	std::vector<ResourceHandle> textureHandles;

	Mesh(std::vector<Vertex> _vertices, std::vector<unsigned int> _indices, float _texIndex) 
		: vertices(_vertices), indices(_indices), texIndex(_texIndex)
	{
		//setupMesh();
	}
	void Draw(Shader& shader);
	void InstancedDraw(Shader& shader, int amount);
	
private:
	unsigned int VBO, EBO;
	void setupMesh();

};

