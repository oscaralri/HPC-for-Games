#pragma once

#include <cstdint>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <unordered_map>
#include <array>

#include "AABB.h"
#include "Mesh.h"
#include "System.h"
#include "Coordinator.h"
#include "ResourceStorage.h"

struct DrawElementsIndirectCommand
{
	uint32_t count; // n indices de la malla 
	uint32_t instanceCount; // cuantas veces dibujar esa malla
	uint32_t firstIndex; // donde empiezan los indices de esa malla en el ebo global 
	uint32_t baseVertex; // desplazamiento en el vertex buffer 
	uint32_t baseInstance; // id de instancia base, se añade al gl_instanceID en el shader

	bool operator==(const DrawElementsIndirectCommand& other) const {
		return count == other.count &&
			instanceCount == other.instanceCount &&
			firstIndex == other.firstIndex &&
			baseVertex == other.baseVertex &&
			baseInstance == other.baseInstance;
	}

	bool operator!=(const DrawElementsIndirectCommand& other) const {
		return !(*this == other);
	}
};

struct MeshEntry
{
	uint32_t baseVertex;   // vbo offset 
	uint32_t firstIndex;    // ebo offset
	uint32_t indexCount;    // n indices
	uint32_t diffuseLayer;
	uint32_t specLayer;
	AABB aabb;
};

struct EntityMeshes
{
	std::vector<MeshEntry> meshEntries;
};

struct InstanceData {
	glm::mat4 modelMatrix;   // 64 bytes 
	uint32_t entityID;       // 4 bytes
	uint32_t diffuseLayer[2];   // 8 bytes  
	uint32_t specLayer;     
	uint32_t cmdIDs[2];      // 8 bytes  
	uint32_t padding1;       // 4 bytes  
	uint32_t padding2;		 // 8 bytes
};


class MDI : public ECS::System
{
public:
	void GenerateMeshBuffers();
	void GenerateDataBuffers();
	MeshEntry AddMesh(ResourceHandle modelRH);
	
	EntityMeshes AddLodsMesh(ResourceHandle modelRH);

	std::vector<DrawElementsIndirectCommand> GenerateDrawCmds(ECS::Coordinator& coordinator);
	uint32_t GenerateDrawCmd(MeshEntry meshEntry);

	GLuint& GetInstanceSSBO() { return instanceSSBO; }
	GLuint& GetCommandsSSBO() { return commandsSSBO; }
	GLuint& GetAABBSSBO() { return aabbSSBO; }
	GLuint& GetDrawCountSSBO() { return drawCountSSBO; }
	GLuint& GetFilteredCmdsSSBO() { return filteredCmdsBuffer; }
	GLuint& GetVisibleIndicesSSBO() { return visibleIndicesSSBO; }

	GLuint& GetGlobalVAO() { return globalVAO; }

	DrawElementsIndirectCommand* GetCommandsPtr() { return commandsPtr; }
	InstanceData* GetInstanceDataPtr() { return instancePtr; }
	AABB* GetAABBPtr() { return aabbPtr; }

private:
	GLuint globalVAO;
	GLuint globalVBO;
	GLuint globalEBO;

	const unsigned int MAX_VERTICES = 5000000;
	const unsigned int MAX_INDICES = 10000000;

	uint32_t currentVertexOffset;
	uint32_t currentIndexOffset;

	GLuint instanceSSBO;
	GLuint commandsSSBO;
	GLuint aabbSSBO;
	GLuint filteredCmdsBuffer;
	GLuint drawCountSSBO;
	GLuint visibleIndicesSSBO;

	DrawElementsIndirectCommand* commandsPtr;
	InstanceData* instancePtr;
	AABB* aabbPtr;

	std::unordered_map<ResourceHandle, MeshEntry> meshEntries;

	std::vector<DrawElementsIndirectCommand> commands;
};

