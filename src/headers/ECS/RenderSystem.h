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
	void UpdateGPUData(ECS::Coordinator& coordinator);
	void RenderMDI(Shader* shader, std::vector<ECS::Entity> entities);
	void RenderGPUCulling(ECS::Coordinator& coordinator, Shader* computeShader, Shader* renderShader);
};
