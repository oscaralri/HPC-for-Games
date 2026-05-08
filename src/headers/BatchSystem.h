#pragma once

#include "ResourceStorage.h"
#include "ECSConfig.h"
#include "Renderable.h"
#include "Coordinator.h"
#include "EngineResources.h"
#include "Transform.h"
#include "Grid.h"

#include <map>


class BatchSystem : public ECS::System
{
public:
	void BuildCellBatches(GridCell& cell, const std::vector<ECS::Entity>& entities, ECS::Coordinator& coordinator);

};

