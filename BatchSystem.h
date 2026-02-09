#pragma once

#include "ResourceStorage.h"
#include "Grid.h"
#include "ECSConfig.h"
#include "Renderable.h"
#include "Coordinator.h"
#include "EngineResources.h"
#include "Transform.h"

#include <map>


struct StaticBatch
{
	unsigned int vao, vbo, ebo;
	unsigned int indexCount;
	ResourceHandle shader; // esto es lo que diferencia cada batch y define si pueden ir juntos
};

class BatchSystem : public ECS::System
{
public:
	void BuildCellBatches(GridCell& cell, const std::vector<ECS::Entity>& entities, ECS::Coordinator& coordinator);

};

