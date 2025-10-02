#pragma once
#include "GameObject.h"
#include <glm/glm.hpp>


void GameObject::checkLODS(glm::vec3 cameraPos)
{
	float dis; // esto es por debug
	if (dis = glm::distance(position, cameraPos) < 25) 
	{
		model.activeLod = 0;
	}
	else
	{
		model.activeLod = 1;
	}
	std::cout << model.activeLod << std::endl;
	/*
	std::cout << "" << std::endl;
	std::cout << position.x << " " << position.y << " " << position.z << std::endl;
	std::cout << "" << std::endl;
	std::cout << " " << cameraPos.x << " " << cameraPos.y << " " << cameraPos.z << std::endl;
	*/
}