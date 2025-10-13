#include "LODSystem.h"

int LODSystem::checkLOD(glm::vec3 position, std::vector<LODLevel> LODs)
{
	size_t i = 0;
	for (; i < LODs.size(); i++)
	{
		if (glm::distance(camera->Position, position) < LODs[i].distance)
		{
			return i;
		}
		
	}

	return i - 1;
}