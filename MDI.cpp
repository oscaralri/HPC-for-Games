#include "MDI.h"

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
	/* FORMA TRADICIONAL
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, instanceSSBO);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, instanceDataList.size() * sizeof(InstanceData), instanceDataList.data());

	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, commandsSSBO);
	glBufferSubData(GL_DRAW_INDIRECT_BUFFER, 0, commands.size() * sizeof(DrawElementsIndirectCommand), commands.data());

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);
	*/

	GLbitfield flags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;

	glGenBuffers(1, &instanceSSBO); // datos de entidades 
	glGenBuffers(1, &commandsSSBO); // ordenes de dibujo (indices, instancia...)

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, instanceSSBO);
	glBufferStorage(GL_SHADER_STORAGE_BUFFER, ECS::MAX_ENTITIES * sizeof(InstanceData), nullptr, flags);
	instancePtr = (InstanceData*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, ECS::MAX_ENTITIES * sizeof(InstanceData), flags);

	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, commandsSSBO);
	glBufferStorage(GL_DRAW_INDIRECT_BUFFER, ECS::MAX_ENTITIES * sizeof(DrawElementsIndirectCommand), nullptr, flags);

	commandsPtr = (DrawElementsIndirectCommand*)glMapBufferRange(GL_DRAW_INDIRECT_BUFFER, 0, 
		ECS::MAX_ENTITIES * sizeof(DrawElementsIndirectCommand), flags);
}

// no se esta gestionando el max vertices ni max indices para que no se escriba en memoria incorrecta
MeshEntry MDI::AddMesh(const std::vector<Vertex>& vertices, std::vector<uint32_t>& indices, AABB& aabb, uint32_t texLayer)
{
	MeshEntry entry;
	entry.baseVertex = currentVertexOffset;
	entry.firstIndex = currentIndexOffset;
	entry.indexCount = indices.size();
	entry.textureLayer = texLayer;
		
	glBindBuffer(GL_ARRAY_BUFFER, globalVBO);
	glBufferSubData(GL_ARRAY_BUFFER, currentVertexOffset * sizeof(Vertex), vertices.size() * sizeof(Vertex), vertices.data());

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, globalEBO);
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, currentIndexOffset * sizeof(uint32_t), indices.size() * sizeof(uint32_t), indices.data());

	currentVertexOffset += (uint32_t)vertices.size();
	currentIndexOffset += (uint32_t)indices.size();

	entry.boundingBox = aabb;

	return entry;
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
		
		// no se añade lo del aabb porque el drawelementsindirectcommand tiene una estructura fija 
		// se hara mas tarde en paralelo

		commands.push_back(cmd);
	}

	return commands;
}