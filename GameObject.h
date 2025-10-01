#pragma once
#include <glm/ext/vector_float3.hpp>

class GameObject
{
private:
	glm::vec3 position; 
public:
	GameObject(glm::vec3 pos) : position(pos) {};
};
