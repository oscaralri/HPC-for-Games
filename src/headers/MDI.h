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
	uint32_t count; //	index number
	uint32_t instanceCount; // number of draws for this mesh
	uint32_t firstIndex; // start of index of this mesh in global ebo 
	uint32_t baseVertex; // offset in vertex data buffer 
	uint32_t baseInstance; // base instance id, gl_InstanceID in shader

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
	glm::mat4 modelMatrix;   
	uint32_t entityID;       
	uint32_t diffuseLayer[2];     
	uint32_t specLayer;     
	uint32_t cmdIDs[2];   
	uint32_t lodActive;
	uint32_t padding;
};


class MDI : public ECS::System
{
public:
	void GenerateMeshBuffers();
	void GenerateDataBuffers();
	
	EntityMeshes AddMesh(ResourceHandle modelRH);
	uint32_t GenerateDrawCmd(MeshEntry meshEntry);

	GLuint& GetInstanceSSBO() { return instanceSSBO; }
	GLuint& GetCommandsSSBO() { return commandsSSBO; }
	GLuint& GetAABBSSBO() { return aabbSSBO; }
	GLuint& GetDrawCountSSBO() { return drawCountACB; }
	GLuint& GetFilteredCmdsSSBO() { return filteredCmdsSSBO; }
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
	GLuint filteredCmdsSSBO;
	GLuint drawCountACB;
	GLuint visibleIndicesSSBO;

	DrawElementsIndirectCommand* commandsPtr;
	InstanceData* instancePtr;
	AABB* aabbPtr;

	std::unordered_map<ResourceHandle, MeshEntry> meshEntries;
	std::vector<DrawElementsIndirectCommand> commands;
};

