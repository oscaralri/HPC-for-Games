#pragma once



#include <memory>
#include <vector>


#include "System.h"
#include "ECSConfig.h"
#include "Camera.h"
#include "Coordinator.h"
#include "AABB.h"
#include "Transform.h"
#include "Renderable.h"

#include "Grid.h"

class CullingSystem : public ECS::System
{
public:
	std::vector<ECS::Entity> FrustumCulling(ECS::Coordinator& coordinator, const std::shared_ptr<Camera>& camera, std::vector<GridCell> cells);

};