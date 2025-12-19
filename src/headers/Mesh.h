#pragma once

#include <glm/glm.hpp>
#include <string>
#include <vector>
#include "Shader.h"
#include <Camera.h>
#include <ResourceStorage.h>
#include "TextureManager.h"
#include "EngineResources.h"

struct Vertex {
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
	glm::vec3 Tangent;
	glm::vec3 Bitangent;
};



class Mesh
{
public:
 // CAMBIAR ESTO A PRIVATE!!!!
	unsigned int VAO;

	std::vector<Vertex> vertices; 
	std::vector<unsigned int> indices;
	std::vector<ResourceHandle> textureHandles;

	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<ResourceHandle> textures) 
		: vertices(vertices), indices(indices), textureHandles(textures)
	{
		setupMesh();
	}
	void Draw(Shader& shader);
	void InstancedDraw(Shader& shader, int amount);

private:
	unsigned int VBO, EBO;
	void setupMesh();

};

