#pragma once
#include "GameObject.h"


void GameObject::Render()
{
	glm::mat4 modelMat = glm::mat4(1.0f);
	modelMat = glm::translate(modelMat, getPosition());
	modelMat = glm::scale(modelMat, getScale());
	modelMat = glm::rotate(modelMat, glm::radians(getRotation().x), glm::vec3(1, 0, 0));
	modelMat = glm::rotate(modelMat, glm::radians(getRotation().y), glm::vec3(0, 1, 0));
	modelMat = glm::rotate(modelMat, glm::radians(getRotation().z), glm::vec3(0, 0, 1));
	shader.use();
	shader.setMat4("model", modelMat);
	
	int lodLevel = OptimizeSystem::getInstance().checkLOD(getPosition(), model.getLODs());
	model.Draw(shader, lodLevel);
}

void GameObject::findAABBMinMax(const std::vector<Mesh> meshes, glm::vec3& aabbMin, glm::vec3& aabbMax)
{
    glm::vec3 maxValues = glm::vec3(std::numeric_limits<float>::lowest());
    glm::vec3 minValues = glm::vec3(std::numeric_limits<float>::max());

    for (const auto& mesh : meshes)
    {
        for (const auto& vertex : mesh.vertices)
        {
            glm::vec4 worldPos = getModelMatrix() * glm::vec4(vertex.Position, 1.0f);
            minValues.x = std::min(minValues.x, worldPos.x);
            minValues.y = std::min(minValues.y, worldPos.y);
            minValues.z = std::min(minValues.z, worldPos.z);

            maxValues.x = std::max(maxValues.x, worldPos.x);
            maxValues.y = std::max(maxValues.y, worldPos.y);
            maxValues.z = std::max(maxValues.z, worldPos.z);
        }
    }

    aabbMin = minValues;
    aabbMax = maxValues;
}
