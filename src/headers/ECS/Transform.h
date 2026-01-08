#pragma once

#include <glm/ext/vector_float3.hpp>

struct Transform
{
	glm::vec3 position;
	glm::vec3 rotation; // rotacion asi implica euler (rotacion para cada eje)
	glm::vec3 scale;
};