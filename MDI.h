#pragma once

#include <cstdint>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "AABB.h"
#include "Mesh.h"
#include "System.h"
#include "Coordinator.h"

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
	AABB boundingBox;
};

// entiendo que lo del alignas(16) es por como va a leer la info la gpu
struct alignas(16) InstanceData {
	uint32_t entityID;
	glm::mat4 modelMatrix; 
	uint32_t textureLayer;   
};

static_assert(sizeof(InstanceData) % 16 == 0, "InstanceData no alineado correctamente para GPU");

class MDI : public ECS::System
{
public:
	void GenerateMeshBuffers();
	void GenerateDataBuffers();
	MeshEntry AddMesh(const std::vector<Vertex>& vertices, std::vector<uint32_t>& indices, AABB& aabb, uint32_t texLayer);
	std::vector<DrawElementsIndirectCommand> GenerateDrawCmds(ECS::Coordinator& coordinator);

	GLuint& GetInstanceSSBO() { return instanceSSBO; }
	GLuint& GetCommandsSSBO() { return commandsSSBO; }
	GLuint& GetGlobalVAO() { return globalVAO; }

private:
	GLuint globalVAO;
	GLuint globalVBO;
	GLuint globalEBO;

	const unsigned int MAX_VERTICES = 500000;
	const unsigned int MAX_INDICES = 500000;

	uint32_t currentVertexOffset;
	uint32_t currentIndexOffset;

	GLuint instanceSSBO;
	GLuint commandsSSBO;
};

