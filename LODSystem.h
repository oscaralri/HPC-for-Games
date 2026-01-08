#pragma once

#include "ECSConfig.h"
#include "Coordinator.h"
#include "Camera.h"
#include "Renderable.h"
#include "Transform.h"

class LODSystem : public ECS::System
{
public:
	void SetLOD(ECS::Coordinator& coordinator, const std::shared_ptr<Camera>& camera, std::vector<ECS::Entity> visibleList);
};

