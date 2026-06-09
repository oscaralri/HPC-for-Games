#include "RenderSystem.h"
#include "ShaderStorage.h"
#include "EngineResources.h"
#include "Application.h"

void RenderSystem::RenderBatch(const StaticBatch& batch)
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D_ARRAY, EngineResources::GetTextureManager().GetGlobalArrayID());

	auto shader = EngineResources::GetShaderManager().Get(batch.shader);
	shader->use();
	shader->setInt("uTextureArray", 0);

	glBindVertexArray(batch.vao);

	glDrawElements(GL_TRIANGLES, batch.indexCount, GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);
}
