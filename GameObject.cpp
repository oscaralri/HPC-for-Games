#pragma once
#include "GameObject.h"


void GameObject::Render()
{
	int lodLevel = LODSystem::getInstance().checkLOD(position);
	//std::cout << "lodLevel " << lodLevel << std::endl;
	model.Draw(shader, lodLevel);
}