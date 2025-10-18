#pragma once
#include "GameObject.h"


void GameObject::Render()
{
	int lodLevel = LODSystem::getInstance().checkLOD(position, model.getLODs());
	model.Draw(shader, lodLevel);
}

bool GameObject::testAABBinFrustum(glm::mat4& MVP, const AABB& aabb)
{
    glm::vec4 corners[8] = {
        {aabb.min.x, aabb.min.y, aabb.min.z, 1.0}, // x y z
        {aabb.max.x, aabb.min.y, aabb.min.z, 1.0}, // X y z
        {aabb.min.x, aabb.max.y, aabb.min.z, 1.0}, // x Y z
        {aabb.max.x, aabb.max.y, aabb.min.z, 1.0}, // X Y z

        {aabb.min.x, aabb.min.y, aabb.max.z, 1.0}, // x y Z
        {aabb.max.x, aabb.min.y, aabb.max.z, 1.0}, // X y Z
        {aabb.min.x, aabb.max.y, aabb.max.z, 1.0}, // x Y Z
        {aabb.max.x, aabb.max.y, aabb.max.z, 1.0}, // X Y Z
    };

    bool inside = false;
    
    size_t size = sizeof(corners) / sizeof(corners[0]);
    for (size_t corner_idx = 0; corner_idx < size; corner_idx++)
    {
        glm::vec4 corner = MVP * corners[corner_idx];

        inside = inside ||
            (within(-corner.w, corner.x, corner.w) &&
                within(-corner.w, corner.y, corner.w) &&
                within(0.0f, corner.z, corner.w));
    }
}

bool within(float min, float val, float max) {
    return val >= min && val <= max;
}