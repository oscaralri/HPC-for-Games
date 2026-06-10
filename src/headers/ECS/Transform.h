#pragma once

#include <glm/glm.hpp>                  
#include <glm/gtc/matrix_transform.hpp> 

struct Transform
{
	glm::vec3 position;
	glm::vec3 rotation; // euler rotation
	glm::vec3 scale;

	glm::mat4 GetModelMatrix()
	{
		glm::mat4 modelMat = glm::mat4(1.0f);

		modelMat = glm::translate(modelMat, position);
		modelMat = glm::scale(modelMat, scale);
		modelMat = glm::rotate(modelMat, glm::radians(rotation.x), glm::vec3(1, 0, 0));
		modelMat = glm::rotate(modelMat, glm::radians(rotation.y), glm::vec3(0, 1, 0));
		modelMat = glm::rotate(modelMat, glm::radians(rotation.z), glm::vec3(0, 0, 1));

		return modelMat;
	}
};

