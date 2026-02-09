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

void RenderSystem::RenderBatch(std::vector<StaticBatch> batches)
{
	for (auto& batch : batches)
	{
		auto shader = EngineResources::GetShaderManager().Get(batch.shader);
		shader->use();

		unsigned int diffuseNr = 1;
		unsigned int specularNr = 1;
		for (unsigned int i = 0; i < textureHandles.size(); i++)
		{
			Texture* texture = EngineResources::GetTextureManager().Get(textureHandles[i]);

			glActiveTexture(GL_TEXTURE0 + i);
			std::string number;
			std::string name = texture->type;
			if (name == "texture_diffuse")
				number = std::to_string(diffuseNr++);
			if (name == "texture_specular")
				number = std::to_string(specularNr++);

			shader->setInt(("material" + name + number).c_str(), i);
			glBindTexture(GL_TEXTURE_2D, texture->id);
		}

		glActiveTexture(GL_TEXTURE0);

		glBindVertexArray(batch.vao);
		glDrawElements(GL_TRIANGLES, batch.indexCount, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}
}