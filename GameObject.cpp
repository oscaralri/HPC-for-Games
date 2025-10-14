#pragma once
#include "GameObject.h"


void GameObject::Render()
{
	int lodLevel = LODSystem::getInstance().checkLOD(position, model.getLODs());
	model.Draw(shader, lodLevel);
}