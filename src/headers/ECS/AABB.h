#pragma once

#include <glm/ext/vector_float4.hpp>

// se cambia a vec4 para alinear en memoria gpu correctamente (16 bytes)
	// el valor w no se utilza realmente
struct AABB
{
	glm::vec4 min;
	glm::vec4 max;
};