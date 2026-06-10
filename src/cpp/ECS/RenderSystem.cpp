#include "RenderSystem.h"
#include "ShaderStorage.h"
#include "EngineResources.h"
#include "Application.h"
#include "MDI.h"

void RenderSystem::UpdateGPUData(ECS::Coordinator& coordinator) {
	auto mdiSystem = coordinator.GetSystem<MDI>();

	InstanceData* gpuInstances = mdiSystem->GetInstanceDataPtr();
	AABB* gpuAABB = mdiSystem->GetAABBPtr();

	for (auto const& entity : mEntities)
	{
		auto& transform = coordinator.GetComponent<Transform>(entity);
		auto& meshEntry = coordinator.GetComponent<EntityMeshes>(entity).meshEntries;

		// instances
		gpuInstances[entity].modelMatrix = transform.GetModelMatrix();
		gpuInstances[entity].specLayer = meshEntry[0].specLayer;
		gpuInstances[entity].entityID = (uint32_t)entity;
		
		// default values 
		gpuInstances[entity].cmdIDs[0] = UINT32_MAX;
		gpuInstances[entity].cmdIDs[1] = UINT32_MAX;
		gpuInstances[entity].diffuseLayer[0] = meshEntry[0].diffuseLayer;
		gpuInstances[entity].diffuseLayer[1] = meshEntry[0].diffuseLayer;

		// draw 
		for (int j = 0; j < meshEntry.size(); j++)
		{
			gpuInstances[entity].cmdIDs[j] = mdiSystem->GenerateDrawCmd(meshEntry[j]);
			gpuInstances[entity].diffuseLayer[j] = meshEntry[j].diffuseLayer;
		}
		
		// aabbs
		gpuAABB[entity] = meshEntry[0].aabb;
	}
}

void RenderSystem::RenderMDI(Shader* shader)
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

	// bind counter and filtered
	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, mdiSystem->GetFilteredCmdsSSBO());
	glBindBuffer(GL_PARAMETER_BUFFER, mdiSystem->GetDrawCountSSBO());
	
	// draw
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
	
	// draw count to zero
	int zero = 0;
	glClearNamedBufferData(mdiSystem->GetDrawCountSSBO(), GL_R32UI, GL_RED_INTEGER, GL_UNSIGNED_INT, &zero);
	
	computeShader->use();
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, mdiSystem->GetInstanceSSBO());
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, mdiSystem->GetAABBSSBO());
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, mdiSystem->GetCommandsSSBO());
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, mdiSystem->GetFilteredCmdsSSBO());
	glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 4, mdiSystem->GetDrawCountSSBO());
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, mdiSystem->GetVisibleIndicesSSBO());

	glDispatchCompute((mEntities.size() + (8 - 1)) / 8, 1, 1); // size elements + shader thread group , -1 because of .size()

	// barrier to let compute shader finish
	glMemoryBarrier(GL_COMMAND_BARRIER_BIT | GL_SHADER_STORAGE_BARRIER_BIT | GL_ATOMIC_COUNTER_BARRIER_BIT);

	RenderMDI(renderShader);
}