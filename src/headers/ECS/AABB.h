#pragma once

#include <glm/ext/vector_float4.hpp>

// vec4 for better memory alignment
struct AABB
{
	glm::vec4 min;
	glm::vec4 max;
};