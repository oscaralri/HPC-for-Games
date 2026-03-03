#include "RenderSystem.h"
#include "ShaderStorage.h"
#include "EngineResources.h"
#include "Application.h"
#include "MDI.h"

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
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D_ARRAY, EngineResources::GetTextureManager().GetGlobalArrayID());

	for (auto& batch : batches)
	{
		auto shader = EngineResources::GetShaderManager().Get(batch.shader);
		shader->use();

		shader->setInt("uTextureArray", 0);	

		glBindVertexArray(batch.vao);

		glDrawElements(GL_TRIANGLES, batch.indexCount, GL_UNSIGNED_INT, 0);

		glBindVertexArray(0);
	}
}

void RenderSystem::UpdateIndirectCmd(ECS::Coordinator& coordinator, std::vector<ECS::Entity> entities) {
	auto mdiSystem = gCoordinator.GetSystem<MDI>();

	InstanceData* gpuInstances = mdiSystem->GetInstanceDataPtr();
	DrawElementsIndirectCommand* gpuCommands = mdiSystem->GetCommandsPtr();

	unsigned int index = 0;

	// quizas si que hago que trabaje con todas las entidades porque el culling se va a hacer en gpu
	for (auto const& entity : entities)
	{
		auto& transform = coordinator.GetComponent<Transform>(entity);
		auto& meshEntry = coordinator.GetComponent<MeshEntry>(entity);

		gpuInstances[index].modelMatrix = transform.GetModelMatrix();
		gpuInstances[index].textureLayer = meshEntry.textureLayer;
		gpuInstances[index].entityID = (uint32_t)entity;

		gpuCommands[index].count = meshEntry.indexCount;
		gpuCommands[index].instanceCount = 1; // 1 para que sea visible
		gpuCommands[index].firstIndex = meshEntry.firstIndex;
		gpuCommands[index].baseVertex = meshEntry.baseVertex;

		gpuCommands[index].baseInstance = index;

		index++;
	}

	std::cout << "\n" << index;
}

void RenderSystem::RenderMDI(Shader& shader, std::vector<ECS::Entity> entities)
{
	auto mdiSystem = gCoordinator.GetSystem<MDI>();
	shader.use();
	glBindVertexArray(mdiSystem->GetGlobalVAO());
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, mdiSystem->GetInstanceSSBO());

	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, mdiSystem->GetCommandsSSBO());

	glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, (void*)0, (GLsizei)entities.size(), 0);

	glBindVertexArray(0);
	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);
}