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

	float TexIndex;
	float SpecIndex;
};

class Mesh
{
public:
	unsigned int VAO;

	std::vector<Vertex> vertices; 
	std::vector<unsigned int> indices;
	float texIndex;
	float specularIndex;

	std::vector<ResourceHandle> textureHandles;

	Mesh(std::vector<Vertex> _vertices, std::vector<unsigned int> _indices, float _texIndex, float _specIndex) 
		: vertices(_vertices), indices(_indices), texIndex(_texIndex), specularIndex(_specIndex)
	{
		//setupMesh();
	}
	void Draw(Shader& shader);
	void InstancedDraw(Shader& shader, int amount);
	
private:
	unsigned int VBO, EBO;
	void setupMesh();

};

