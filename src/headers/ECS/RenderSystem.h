#pragma once

#include "Coordinator.h"
#include "System.h"
#include "Transform.h"
#include "Renderable.h"
#include "ECSConfig.h"
#include "BatchSystem.h"
#include "MDI.h"


class RenderSystem : public ECS::System
{
public:
	void Render(ECS::Coordinator& coordinator, std::vector<ECS::Entity>& entities);
	void RenderInstanced(ECS::Coordinator& coordinator, std::vector<ECS::Entity>& entities);
	void RenderBatch(std::vector<StaticBatch> batches);
	void UpdateIndirectCmd(ECS::Coordinator& coordinator, std::vector<ECS::Entity> entities);
	void RenderMDI(Shader& shader, std::vector<ECS::Entity> entities);

};
