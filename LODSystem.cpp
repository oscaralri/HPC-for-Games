#include "LODSystem.h"


int LODSystem::checkLOD(glm::vec3 position)
{
	if (glm::distance(camera.Position, position) < 25)
	{
		return 0;
	}
	else
	{
		return 1;
	}
}