#pragma once
#include <glm/ext/vector_float3.hpp>
#include "Model.h"

/*
struct LODLevel {
	std::vector<Mesh> meshes;
};
*/

class GameObject
{
private:
	// define cuantos niveles y por tanto meshes diferentes hay

	//std::vector<LODLevel> LODs;
	Model& model;

public:
	glm::vec3 position; 
	
	void Render();

};
