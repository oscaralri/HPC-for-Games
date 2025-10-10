#pragma once
#include "GameObject.h"


void GameObject::Render()
{
	int lodLevel = LODSystem::getInstance().checkLOD(position);
	std::cout << "lodLevel " << lodLevel;
	model.Draw(shader, lodLevel);
}