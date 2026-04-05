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

	int counter = 0;

	for (auto const& entity : mEntities)
	{
		auto& transform = coordinator.GetComponent<Transform>(entity);
		// CREO QUE DEBERIA SERVIR USAR LA DEL 0 PORQUE AL FINAL ES INFO QUE SIRVE PARA LOS DOS Y ES UN VALOR DEFAULT
			// QUE SE GESTIONA DESPUES EN EL COMPUTE SHADER
		auto& meshEntry = coordinator.GetComponent<EntityMeshes>(entity).meshEntries;

		// instances
		gpuInstances[entity].modelMatrix = transform.GetModelMatrix();
		gpuInstances[entity].textureLayer = meshEntry[0].textureLayer;
		gpuInstances[entity].entityID = (uint32_t)entity;

		// ESTO MIRARLO PORQUE CREO QUE PODRIA LLEGAR A QUITARLO DE AQUI CUANDO LODS FUNCIONE
		// draw 
		for (int j = 0; j < meshEntry.size(); j++)
		{
			gpuInstances[entity].cmdIDs[j] = mdiSystem->GenerateDrawCmd(meshEntry[j]);
		}

		/*
		for (int j = 0; j < meshEntry.size(); j++)
		{
			gpuCommands[counter].count = meshEntry[j].indexCount;
			gpuCommands[counter].instanceCount = 1; // 1: visible, 0: no 
			gpuCommands[counter].firstIndex = meshEntry[j].firstIndex;
			gpuCommands[counter].baseVertex = meshEntry[j].baseVertex;
			gpuCommands[counter].baseInstance = entity; // relaciona con gl_DrawID / el ECS::Entity que es lo que se devuelve es un unsigned int al fin y al cabo
			++counter;
		}
		*/
		/*
		gpuCommands[entity].count = meshEntry[0].indexCount;
		gpuCommands[entity].instanceCount = 1; // 1 para que sea visible , SERIA MEJOR INICIALIZAR A 0 PERO POR AHORA SE USA ASI PARA DEBUG
		gpuCommands[entity].firstIndex = meshEntry[0].firstIndex;
		gpuCommands[entity].baseVertex = meshEntry[0].baseVertex;
		gpuCommands[entity].baseInstance = entity;
		*/

		
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
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, mdiSystem->GetVisibleIndicesSSBO());

	glDispatchCompute((mEntities.size() + 7) / 8, 1, 1); // tamanyo elementos + grupo en shader , - 1 por size

	// barrera para que compute shader acabe
	glMemoryBarrier(GL_COMMAND_BARRIER_BIT | GL_SHADER_STORAGE_BARRIER_BIT | GL_ATOMIC_COUNTER_BARRIER_BIT);

	RenderMDI(renderShader, mEntities);
}