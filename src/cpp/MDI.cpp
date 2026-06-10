#include "MDI.h"

#include "EngineResources.h"

void MDI::GenerateMeshBuffers()
{
	glGenVertexArrays(1, &globalVAO);
	glBindVertexArray(globalVAO);

	glGenBuffers(1, &globalVBO);
	glBindBuffer(GL_ARRAY_BUFFER, globalVBO);
	glBufferData(GL_ARRAY_BUFFER, MAX_VERTICES * sizeof(Vertex), nullptr, GL_STATIC_DRAW);

	glGenBuffers(1, &globalEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, globalEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, MAX_INDICES * sizeof(uint32_t), nullptr, GL_STATIC_DRAW);

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

	glGenBuffers(1, &instanceSSBO); // entity data
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, instanceSSBO);
	glBufferStorage(GL_SHADER_STORAGE_BUFFER, ECS::MAX_ENTITIES * sizeof(InstanceData), nullptr, flags);
	instancePtr = (InstanceData*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, ECS::MAX_ENTITIES * sizeof(InstanceData), flags);

	glGenBuffers(1, &commandsSSBO); // draw commands
	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, commandsSSBO);
	glBufferStorage(GL_DRAW_INDIRECT_BUFFER, ECS::MAX_ENTITIES * sizeof(DrawElementsIndirectCommand), nullptr, flags);
	commandsPtr = (DrawElementsIndirectCommand*)glMapBufferRange(GL_DRAW_INDIRECT_BUFFER, 0, 
		ECS::MAX_ENTITIES * sizeof(DrawElementsIndirectCommand), flags);

	glGenBuffers(1, &aabbSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, aabbSSBO);
	glBufferStorage(GL_SHADER_STORAGE_BUFFER, ECS::MAX_ENTITIES * sizeof(AABB), nullptr, flags);
	aabbPtr = (AABB*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, ECS::MAX_ENTITIES * sizeof(AABB), flags);

	glGenBuffers(1, &filteredCmdsSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, filteredCmdsSSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, ECS::MAX_ENTITIES * sizeof(DrawElementsIndirectCommand), nullptr, GL_DYNAMIC_DRAW);

	glGenBuffers(1, &visibleIndicesSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, visibleIndicesSSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(GLuint) * ECS::MAX_ENTITIES, NULL, GL_DYNAMIC_DRAW);

	glGenBuffers(1, &drawCountACB);
	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, drawCountACB);
	glBufferData(GL_ATOMIC_COUNTER_BUFFER, sizeof(GLuint), NULL, GL_DYNAMIC_DRAW);
}

EntityMeshes MDI::AddMesh(ResourceHandle modelRH)
{
	EntityMeshes entityMeshes;
	auto model = EngineResources::GetModelManager().Get(modelRH);

	for (int i = 0; i < model->getLODs().size(); i++)
	{
		AABB aabb
		{
			glm::vec4(EngineResources::GetModelManager().Get(modelRH)->getMinMax()[0], 0), // we add 0 to use vec4 but the value is not used
			glm::vec4(EngineResources::GetModelManager().Get(modelRH)->getMinMax()[1], 0)
		};

		// only a mesh for each lod level because of simplicity
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;
		vertices = model->getLODs()[i].meshes[0].vertices;
		indices = model->getLODs()[i].meshes[0].indices;

		MeshEntry entry;
		entry.baseVertex = currentVertexOffset;
		entry.firstIndex = currentIndexOffset;
		entry.indexCount = indices.size();
		entry.diffuseLayer = model->getLODs()[i].meshes[0].texIndex;
		entry.specLayer = model->getLODs()[i].meshes[0].specularIndex;
		entry.aabb = aabb;

		glBindBuffer(GL_ARRAY_BUFFER, globalVBO);
		glBufferSubData(GL_ARRAY_BUFFER, currentVertexOffset * sizeof(Vertex), vertices.size() * sizeof(Vertex), vertices.data());

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, globalEBO);
		glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, currentIndexOffset * sizeof(uint32_t), indices.size() * sizeof(uint32_t), indices.data());

		entityMeshes.meshEntries.push_back(entry);

		currentVertexOffset += (uint32_t)vertices.size();
		currentIndexOffset += (uint32_t)indices.size();
	}

	return entityMeshes;
}

uint32_t MDI::GenerateDrawCmd(MeshEntry meshEntry)
{
	DrawElementsIndirectCommand cmd;
	cmd.count = meshEntry.indexCount;
	cmd.instanceCount = 1;
	cmd.firstIndex = meshEntry.firstIndex;
	cmd.baseVertex = meshEntry.baseVertex;
	cmd.baseInstance = 0;

	for (int i = 0; i < commands.size(); i++)
	{
		if (cmd == commands[i]) return i;
	}

	commands.push_back(cmd);

	auto gpuCommands = GetCommandsPtr();
	gpuCommands[commands.size() - 1] = cmd;

	return commands.size() - 1;
}