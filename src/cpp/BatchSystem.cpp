#include "BatchSystem.h"

StaticBatch UploadToGPU(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices, ResourceHandle shader)
{
	StaticBatch batch;
	batch.shader = shader;
	batch.indexCount = indices.size();

	glGenVertexArrays(1, &batch.vao);
	glGenBuffers(1, &batch.vbo);
	glGenBuffers(1, &batch.ebo);

	glBindVertexArray(batch.vao);

	glBindBuffer(GL_ARRAY_BUFFER, batch.vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, batch.ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

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

	glBindVertexArray(0);

	return batch;
}

void BatchSystem::BuildCellBatches(GridCell& cell, const std::vector<ECS::Entity>& entities, ECS::Coordinator& coordinator)
{
	for (int lodLevel = 0; lodLevel < 1; lodLevel++)
	{
		std::map<ResourceHandle, std::vector<ECS::Entity>> groups;
		for (auto& entity : entities)
		{
			auto& renderable = coordinator.GetComponent<Renderable>(entity);

			if (renderable.renderType == RenderType::Batch)
			{
				auto model = EngineResources::GetModelManager().Get(renderable.model);
				if (model->getLODs().size() > lodLevel)
				{
					groups[renderable.shader].push_back(entity);
				}
			}
		}

		for (auto& [shader, groupEntities] : groups)
		{
			std::vector<Vertex> mergedVertices;
			std::vector<unsigned int> mergedIndices;
			unsigned int vertexOffset = 0;

			for (auto& entity : groupEntities)
			{
				auto renderable = coordinator.GetComponent<Renderable>(entity);
				auto transform = coordinator.GetComponent<Transform>(entity);
				Model* model = EngineResources::GetModelManager().Get(renderable.model);
				glm::mat4 modelMat = transform.GetModelMatrix();
				glm::mat3 normalMat = glm::mat3(glm::transpose(glm::inverse(modelMat)));

				for (auto& mesh : model->getLODs()[lodLevel].meshes)
				{
					for (const auto& v : mesh.vertices)
					{
						Vertex vWorld = v;
						vWorld.Position = glm::vec3(modelMat * glm::vec4(v.Position, 1.0f));
						vWorld.Normal = glm::normalize(normalMat * v.Normal); 
						vWorld.TexIndex = mesh.texIndex;

						mergedVertices.push_back(vWorld);
					}

					for (unsigned int idx : mesh.indices)
					{
						mergedIndices.push_back(idx + vertexOffset);
					}
					vertexOffset += (unsigned int)mesh.vertices.size();
				}
			}

			cell.lodBatches[lodLevel].push_back(UploadToGPU(mergedVertices, mergedIndices, shader));
		}
	}
}
