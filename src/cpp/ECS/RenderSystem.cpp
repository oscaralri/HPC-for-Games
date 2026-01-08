#include "RenderSystem.h"
#include "ShaderStorage.h"
#include "EngineResources.h"
#include "Application.h"

void RenderSystem::Render(ECS::Coordinator& coordinator, std::vector<ECS::Entity>& entities)
{
	for (auto const& entity : entities)
	{
		auto& transform = coordinator.GetComponent<Transform>(entity);
		auto& renderable = coordinator.GetComponent<Renderable>(entity);

		auto model = EngineResources::GetModelManager().Get(renderable.model);
		auto shader = EngineResources::GetShaderManager().Get(renderable.shader);
		
		glm::mat4 modelMat = glm::mat4(1.0f);

		modelMat = glm::translate(modelMat, transform.position);
		modelMat = glm::scale(modelMat, transform.scale);
		modelMat = glm::rotate(modelMat, glm::radians(transform.rotation.x), glm::vec3(1, 0, 0));
		modelMat = glm::rotate(modelMat, glm::radians(transform.rotation.y), glm::vec3(0, 1, 0));
		modelMat = glm::rotate(modelMat, glm::radians(transform.rotation.z), glm::vec3(0, 0, 1));

		shader->use();
		shader->setMat4("model", modelMat);

		model->Draw(*shader, renderable.LodLevel);
	}
}

void RenderSystem::RenderInstanced(ECS::Coordinator& coordinator, std::vector<ECS::Entity>& entities)
{
	auto& renderable = coordinator.GetComponent<Renderable>(entities[0]);

	auto model = EngineResources::GetModelManager().Get(renderable.model);
	auto shader = EngineResources::GetShaderManager().Get(renderable.shader);

	model->InstancedDraw(*shader, renderable.LodLevel, entities.size());
}