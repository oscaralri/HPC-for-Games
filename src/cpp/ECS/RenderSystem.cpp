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
		// CREO QUE DEBERIA SERVIR USAR LA DEL 0 PORQUE AL FINAL ES INFO QUE SIRVE PARA LOS DOS Y ES UN VALOR DEFAULT
			// QUE SE GESTIONA DESPUES EN EL COMPUTE SHADER
		auto& meshEntry = coordinator.GetComponent<EntityMeshes>(entity).meshEntries[0];

		// instances
		gpuInstances[entity].modelMatrix = transform.GetModelMatrix();
		gpuInstances[entity].textureLayer = meshEntry.textureLayer;
		gpuInstances[entity].entityID = (uint32_t)entity;

		// ESTO MIRARLO PORQUE CREO QUE PODRIA LLEGAR A QUITARLO DE AQUI 
		// draw cmds
		gpuCommands[entity].count = meshEntry.indexCount;
		gpuCommands[entity].instanceCount = 1; // 1 para que sea visible , SERIA MEJOR INICIALIZAR A 0 PERO POR AHORA SE USA ASI PARA DEBUG
		gpuCommands[entity].firstIndex = meshEntry.firstIndex;
		gpuCommands[entity].baseVertex = meshEntry.baseVertex;
		gpuCommands[entity].baseInstance = entity;

		// aabbs
		gpuAABB[entity] = meshEntry.aabb;
	}
}

void RenderSystem::RenderMDI(Shader* shader, std::vector<ECS::Entity> entities)
{
	auto mdiSystem = gCoordinator.GetSystem<MDI>();

	shader->use();	
	glBindVertexArray(mdiSystem->GetGlobalVAO());
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, mdiSystem->GetInstanceSSBO());
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 6, mdiSystem->GetVisibleIndicesSSBO());

	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, mdiSystem->GetFilteredCmdsSSBO());

	glBindBuffer(GL_PARAMETER_BUFFER, mdiSystem->GetDrawCountSSBO());
	
	glMultiDrawElementsIndirectCount(GL_TRIANGLES, GL_UNSIGNED_INT, (void*)0, 0, 100 /*ECS::MAX_ENTITIES*/, 0);

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
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 6, mdiSystem->GetVisibleIndicesSSBO());

	glDispatchCompute(/*(mEntities.size() + 8 - 1)*/(mEntities.size() + 7) / 8, 1, 1); // tamanyo elementos , + grupo en shader , - 1 por size

	// barrera para que compute shader acabe
	glMemoryBarrier(GL_COMMAND_BARRIER_BIT | GL_SHADER_STORAGE_BARRIER_BIT | GL_ATOMIC_COUNTER_BARRIER_BIT);

	RenderMDI(renderShader, mEntities);
}