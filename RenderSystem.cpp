#include "RenderSystem.h"

void RenderSystem::Render(ECS::Coordinator& coordinator)
{
	for (auto const& entity : mEntities)
	{
		auto& transform = coordinator.GetComponent<TransformECS>(entity);
		auto& renderable = coordinator.GetComponent<Renderable>(entity);

		glm::mat4 modelMat = glm::mat4(1.0f);
		modelMat = glm::translate(modelMat, transform.position);
		modelMat = glm::scale(modelMat, transform.scale);
		modelMat = glm::rotate(modelMat, glm::radians(transform.rotation.x), glm::vec3(1, 0, 0));
		modelMat = glm::rotate(modelMat, glm::radians(transform.rotation.y), glm::vec3(0, 1, 0));
		modelMat = glm::rotate(modelMat, glm::radians(transform.rotation.z), glm::vec3(0, 0, 1));

		renderable.shader->use();
		renderable.shader->setMat4("model", modelMat);

		renderable.model->Draw(*renderable.shader, 0);
	}
}