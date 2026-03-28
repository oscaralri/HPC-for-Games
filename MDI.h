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
};

struct MeshEntry
{
	uint32_t baseVertex;   // vbo offset 
	uint32_t firstIndex;    // ebo offset
	uint32_t indexCount;    // n indices
	uint32_t textureLayer;
	AABB aabb;
};

struct EntityMeshes
{
	std::array<MeshEntry, 2> meshEntries;
};

// entiendo que lo del alignas(16) es por como va a leer la info la gpu
struct InstanceData {
	glm::mat4 modelMatrix;   // 64 bytes 
	uint32_t entityID;       // 4 bytes
	uint32_t textureLayer;   // 4 bytes  
	uint32_t pad0;
	uint32_t pad1; // 8 bytes		
};


class MDI : public ECS::System
{
public:
	void GenerateMeshBuffers();
	void GenerateDataBuffers();
	MeshEntry AddMesh(ResourceHandle modelRH);
	
	EntityMeshes AddLodsMesh(ResourceHandle modelRH);

	std::vector<DrawElementsIndirectCommand> GenerateDrawCmds(ECS::Coordinator& coordinator);

	GLuint& GetInstanceSSBO() { return instanceSSBO; }
	GLuint& GetCommandsSSBO() { return commandsSSBO; }
	GLuint& GetAABBSSBO() { return aabbSSBO; }
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

	DrawElementsIndirectCommand* commandsPtr;
	InstanceData* instancePtr;
	AABB* aabbPtr;

	std::unordered_map<ResourceHandle, MeshEntry> meshEntries;
};

