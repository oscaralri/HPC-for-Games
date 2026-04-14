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

	for (auto const& entity : mEntities)
	{
		auto& transform = coordinator.GetComponent<Transform>(entity);
		auto& meshEntry = coordinator.GetComponent<EntityMeshes>(entity).meshEntries;

		// instances
		gpuInstances[entity].modelMatrix = transform.GetModelMatrix();
		gpuInstances[entity].textureLayer = meshEntry[0].textureLayer;
		gpuInstances[entity].specLayer = meshEntry[0].specLayer;
		gpuInstances[entity].entityID = (uint32_t)entity;
		gpuInstances[entity].cmdIDs[0] = UINT32_MAX;
		gpuInstances[entity].cmdIDs[1] = UINT32_MAX;

		// draw 
		for (int j = 0; j < meshEntry.size(); j++)
		{
			gpuInstances[entity].cmdIDs[j] = mdiSystem->GenerateDrawCmd(meshEntry[j]);
		}
		
		// aabbs
		gpuAABB[entity] = meshEntry[0].aabb;
	}
}

void RenderSystem::RenderMDI(Shader* shader, std::vector<ECS::Entity> entities)
{
	auto mdiSystem = gCoordinator.GetSystem<MDI>();

	shader->use();	
	glBindVertexArray(mdiSystem->GetGlobalVAO());
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, mdiSystem->GetInstanceSSBO());
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, mdiSystem->GetVisibleIndicesSSBO());
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D_ARRAY, EngineResources::GetTextureManager().GetTextureArrayID(Diffuse));
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D_ARRAY, EngineResources::GetTextureManager().GetTextureArrayID(Specular));

	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, mdiSystem->GetFilteredCmdsSSBO());

	glBindBuffer(GL_PARAMETER_BUFFER, mdiSystem->GetDrawCountSSBO());
	
	glMultiDrawElementsIndirectCount(GL_TRIANGLES, GL_UNSIGNED_INT, (void*)0, 0, ECS::MAX_ENTITIES, 0);

	// reset to default
	glActiveTexture(GL_TEXTURE1); 
	glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
	glActiveTexture(GL_TEXTURE0); 
	glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
	glBindVertexArray(0);
	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);
}

void RenderSystem::RenderGPUCulling(ECS::Coordinator& coordinator, Shader* computeShader, Shader* renderShader)
{
	auto mdiSystem = coordinator.GetSystem<MDI>();
	
	int zero = 0;
	glClearNamedBufferData(mdiSystem->GetDrawCountSSBO(), GL_R32UI, GL_RED_INTEGER, GL_UNSIGNED_INT, &zero);
	
	//glMemoryBarrier(GL_BUFFER_UPDATE_BARRIER_BIT | GL_ATOMIC_COUNTER_BARRIER_BIT);

	computeShader->use();
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, mdiSystem->GetInstanceSSBO());
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, mdiSystem->GetAABBSSBO());
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, mdiSystem->GetCommandsSSBO());
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, mdiSystem->GetFilteredCmdsSSBO());
	glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 4, mdiSystem->GetDrawCountSSBO());
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, mdiSystem->GetVisibleIndicesSSBO());

	glDispatchCompute((mEntities.size() + 7) / 8, 1, 1); // tamanyo elementos + grupo en shader , - 1 por size

	// barrera para que compute shader acabe
	glMemoryBarrier(GL_COMMAND_BARRIER_BIT | GL_SHADER_STORAGE_BARRIER_BIT | GL_ATOMIC_COUNTER_BARRIER_BIT);

	RenderMDI(renderShader, mEntities);
}