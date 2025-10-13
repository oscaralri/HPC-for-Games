#include "LODSystem.h"


int LODSystem::checkLOD(glm::vec3 position)
{
	//std::cout << camera.Position.x << camera.Position.y << camera.Position.z << std::endl;
	if (glm::distance(camera->Position, position) < 25)
	{
		return 0;
	}
	else
	{
		return 1;
	}
}