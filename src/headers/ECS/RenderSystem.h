#pragma once

#include "Coordinator.h"
#include "System.h"
#include "Transform.h"
#include "Renderable.h"
#include "ECSConfig.h"


class RenderSystem : public ECS::System
{
public:
	void Render(ECS::Coordinator& coordinator, std::vector<ECS::Entity>& entities);
	void RenderInstanced(ECS::Coordinator& coordinator, std::vector<ECS::Entity>& entities);
};

