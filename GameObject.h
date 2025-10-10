#pragma once
#include <glm/ext/vector_float3.hpp>
#include "Model.h"
#include "LODSystem.h"

class GameObject
{
private:
	glm::vec3 position;
	Model& model;
	Shader& shader;

public:
	GameObject(glm::vec3 p, Model m, Shader s) : position(p), model(m), shader(s) {};
	
	glm::vec3 getPosition() { return position; }
	void Render();
};
 