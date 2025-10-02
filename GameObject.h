#pragma once
#include <glm/ext/vector_float3.hpp>
#include "Model.h"

class GameObject
{
private:
	
public:
	glm::vec3 position; // ESTO DEBERIA SER PRIVATE !!!!!!
	Model& model;

	GameObject(glm::vec3 pos, Model& mod) : position(pos), model(mod) {};

	glm::vec3 getPosition() { return position; }

	void checkLODS(glm::vec3 cameraPos);
};
