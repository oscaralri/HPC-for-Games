#include "RenderSystem.h"
#include "ShaderStorage.h"
#include "EngineResources.h"
#include "Application.h"
#include "MDI.h"

void RenderSystem::UpdateIndirectCmd(ECS::Coordinator& coordinator) {
	auto mdiSystem = coordinator.GetSystem<MDI>();

	InstanceData* gpuInstances = mdiSystem->GetInstanceDataPtr();
	DrawElementsIndirectCommand* gpuCommands = mdiSystem->GetCommandsPtr();
	AABB* gpuAABB = mdiSystem->GetAABBPtr();

	unsigned int index = 0;

	for (auto const& entity : mEntities)
	{
		auto& transform = coordinator.GetComponent<Transform>(entity);
		auto& meshEntry = coordinator.GetComponent<MeshEntry>(entity);

		// instances
		gpuInstances[index].modelMatrix = transform.GetModelMatrix();
		gpuInstances[index].textureLayer = meshEntry.textureLayer;
		gpuInstances[index].entityID = (uint32_t)entity;

		// aabbs
		gpuAABB[index] = meshEntry.aabb;

		// draw cmds
		gpuCommands[index].count = meshEntry.indexCount;
		gpuCommands[index].instanceCount = 1; // 1 para que sea visible , SERIA MEJOR INICIALIZAR A 0 PERO POR AHORA SE USA ASI PARA DEBUG
		gpuCommands[index].firstIndex = meshEntry.firstIndex;
		gpuCommands[index].baseVertex = meshEntry.baseVertex;
		gpuCommands[index].baseInstance = index;

		++index;
	}
}

void RenderSystem::RenderMDI(Shader* shader, std::vector<ECS::Entity> entities)
{
	auto mdiSystem = gCoordinator.GetSystem<MDI>();

	shader->use(); 
	glBindVertexArray(mdiSystem->GetGlobalVAO());
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, mdiSystem->GetInstanceSSBO());

	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, mdiSystem->GetCommandsSSBO());

	glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, (void*)0, (GLsizei)entities.size(), 0);
	
	glBindVertexArray(0);
	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);
}

void RenderSystem::RenderGPUCulling(ECS::Coordinator& coordinator, Shader* computeShader, Shader* renderShader)
{
	auto mdiSystem = coordinator.GetSystem<MDI>();
	
	computeShader->use();
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, mdiSystem->GetInstanceSSBO());
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, mdiSystem->GetAABBSSBO());
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, mdiSystem->GetCommandsSSBO());
	
	glDispatchCompute((mEntities.size() + 8 - 1), 1, 1); // tamanyo elementos , + grupo en shader , - 1 por size
	
	// barrera para que compute shader acabe
	glMemoryBarrier(GL_COMMAND_BARRIER_BIT);

	RenderMDI(renderShader, mEntities);
}