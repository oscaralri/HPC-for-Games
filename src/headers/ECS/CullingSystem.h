#pragma once

#include "System.h"
#include "ECSConfig.h"
#include "Camera.h"
#include "Coordinator.h"


#include <AABB.h>
#include <memory>
#include <vector>
#include <Transform.h>
#include <Renderable.h>

class CullingSystem : public ECS::System
{
public:
	std::vector<ECS::Entity> FrustumCulling(ECS::Coordinator& coordinator, const std::shared_ptr<Camera>& camera);

};