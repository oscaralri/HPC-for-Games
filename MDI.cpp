#include "MDI.h"

#include "EngineResources.h"

// reservamos dos buffers con un gran tamano que se iran rellenando con los meshes
void MDI::GenerateMeshBuffers()
{
	glGenVertexArrays(1, &globalVAO);
	glBindVertexArray(globalVAO);

	// vbo
	glGenBuffers(1, &globalVBO);
	glBindBuffer(GL_ARRAY_BUFFER, globalVBO);
	glBufferData(GL_ARRAY_BUFFER, MAX_VERTICES * sizeof(Vertex), nullptr, GL_STATIC_DRAW);

	// ebo 
	glGenBuffers(1, &globalEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, globalEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, MAX_INDICES * sizeof(uint32_t), nullptr, GL_STATIC_DRAW);

	// atributos
	// vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	// vertex normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
	// vertex texcoords
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
	// vertex texIndex
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexIndex));

	currentVertexOffset = 0;
	currentIndexOffset = 0;
}

void MDI::GenerateDataBuffers()
{
	GLbitfield flags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;

	glGenBuffers(1, &instanceSSBO); // datos de entidades 
	glGenBuffers(1, &commandsSSBO); // ordenes de dibujo (indices, instancia...)
	glGenBuffers(1, &aabbSSBO);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, instanceSSBO);
	glBufferStorage(GL_SHADER_STORAGE_BUFFER, ECS::MAX_ENTITIES * sizeof(InstanceData), nullptr, flags);
	instancePtr = (InstanceData*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, ECS::MAX_ENTITIES * sizeof(InstanceData), flags);

	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, commandsSSBO);
	glBufferStorage(GL_DRAW_INDIRECT_BUFFER, ECS::MAX_ENTITIES * sizeof(DrawElementsIndirectCommand), nullptr, flags);
	commandsPtr = (DrawElementsIndirectCommand*)glMapBufferRange(GL_DRAW_INDIRECT_BUFFER, 0, 
		ECS::MAX_ENTITIES * sizeof(DrawElementsIndirectCommand), flags);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, aabbSSBO);
	glBufferStorage(GL_SHADER_STORAGE_BUFFER, ECS::MAX_ENTITIES * sizeof(AABB), nullptr, flags);
	aabbPtr = (AABB*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, ECS::MAX_ENTITIES * sizeof(AABB), flags);
}

MeshEntry MDI::AddMesh(ResourceHandle modelRH)
{
	auto it = meshEntries.find(modelRH);

	if (it == meshEntries.end())
	{
		auto model = EngineResources::GetModelManager().Get(modelRH);
		AABB aabb
		{
			glm::vec4(EngineResources::GetModelManager().Get(modelRH)->getMinMax()[0], 0), // se anade 0 para cumplir con vec4 pero no se utilizara ese valor
			glm::vec4(EngineResources::GetModelManager().Get(modelRH)->getMinMax()[1], 0)
		};

		std::vector<Vertex> vertices = model->getLODs()[0].meshes[0].vertices;
		std::vector<unsigned int> indices = model->getLODs()[0].meshes[0].indices;

		MeshEntry entry;
		entry.baseVertex = currentVertexOffset;
		entry.firstIndex = currentIndexOffset;
		entry.indexCount = indices.size();
		entry.textureLayer = model->getLODs()[0].meshes[0].texIndex;
		entry.aabb = aabb;

		glBindBuffer(GL_ARRAY_BUFFER, globalVBO);
		glBufferSubData(GL_ARRAY_BUFFER, currentVertexOffset * sizeof(Vertex), vertices.size() * sizeof(Vertex), vertices.data());

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, globalEBO);
		glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, currentIndexOffset * sizeof(uint32_t), indices.size() * sizeof(uint32_t), indices.data());

		meshEntries.insert({modelRH, entry});

		currentVertexOffset += (uint32_t)vertices.size();
		currentIndexOffset += (uint32_t)indices.size();

		return entry;
	}

	return it->second;
}

std::vector<DrawElementsIndirectCommand> MDI::GenerateDrawCmds(ECS::Coordinator& coordinator)
{
	std::vector<DrawElementsIndirectCommand> commands;

	for (int i = 0; i < mEntities.size(); i++)
	{
		auto& meshEntry = coordinator.GetComponent<MeshEntry>(mEntities[i]);

		DrawElementsIndirectCommand cmd;
		cmd.count = meshEntry.indexCount;
		cmd.instanceCount = 1; // 1: visible, 0: no 
		cmd.firstIndex = meshEntry.firstIndex;
		cmd.baseVertex = meshEntry.baseVertex;
		cmd.baseInstance = mEntities[i]; // relaciona con gl_DrawID / el ECS::Entity que es lo que se devuelve es un unsigned int al fin y al cabo

		commands.push_back(cmd);
	}

	return commands;
}