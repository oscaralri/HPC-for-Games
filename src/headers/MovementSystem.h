#pragma once

#include "System.h"
#include "Coordinator.h"
#include "Movement.h"
#include "Transform.h"
#include "MDI.h"

class MovementSystem : public ECS::System {
public:
	void UpdatePositions(ECS::Coordinator& coordinator)
	{
		for (auto& entity : mEntities)
		{
			auto& movement = coordinator.GetComponent<Movement>(entity);
			auto& transform = coordinator.GetComponent<Transform>(entity);

			glm::vec3 auxPosition = transform.position + movement.velocity;
			
			bool outOfBounds = false;

			if (movement.velocity.x > 0 && transform.position.x > movement.maxPosition.x) outOfBounds = true;
			else if (movement.velocity.x < 0 && transform.position.x < movement.maxPosition.x) outOfBounds = true;

			if (movement.velocity.z > 0 && transform.position.z > movement.maxPosition.z) outOfBounds = true;
			else if (movement.velocity.z < 0 && transform.position.z < movement.maxPosition.z) outOfBounds = true;
			
			if (outOfBounds)
			{
				transform.position = movement.initPosition;
			}
			else
			{
				transform.position = auxPosition;
			}

			UpdateGPUData(coordinator, entity, transform);
		}
	}

private:
	void UpdateGPUData(ECS::Coordinator& coordinator, ECS::Entity entity, Transform transform)
	{
		auto mdiSystem = coordinator.GetSystem<MDI>();

		InstanceData* gpuInstances = mdiSystem->GetInstanceDataPtr();
		gpuInstances[entity].modelMatrix = transform.GetModelMatrix();
	}
};