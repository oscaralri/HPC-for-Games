#pragma once
#include "GameObject.h"


void GameObject::Render(Shader& s)
{
	glm::mat4 modelMat = glm::mat4(1.0f);
	modelMat = glm::translate(modelMat, getPosition());
	modelMat = glm::scale(modelMat, getScale());
	modelMat = glm::rotate(modelMat, glm::radians(getRotation().x), glm::vec3(1, 0, 0));
	modelMat = glm::rotate(modelMat, glm::radians(getRotation().y), glm::vec3(0, 1, 0));
	modelMat = glm::rotate(modelMat, glm::radians(getRotation().z), glm::vec3(0, 0, 1));
	s.use();
	s.setMat4("model", modelMat);
	
	int lodLevel = LODSystem::getInstance().checkLOD(getPosition(), model.getLODs());
	model.Draw(shader, lodLevel);
}