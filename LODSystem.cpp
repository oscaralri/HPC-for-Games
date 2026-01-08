#include "LODSystem.h"

#include "EngineResources.h"

int CheckLOD(glm::vec3 position, std::vector<LODLevel> LODs, const std::shared_ptr<Camera>& camera)
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

void LODSystem::SetLOD(ECS::Coordinator& coordinator, 
	const std::shared_ptr<Camera>& camera, std::vector<ECS::Entity> visibleList)
{
	for (const auto& entity : visibleList)
	{
		auto model = EngineResources::GetModelManager().Get(coordinator.GetComponent<Renderable>(entity).model);
		coordinator.GetComponent<Renderable>(entity).LodLevel = CheckLOD(coordinator.GetComponent<Transform>(entity).position, model->getLODs(), camera);
	}
}

