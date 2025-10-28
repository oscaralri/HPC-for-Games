#include "OptimizeSystem.h"

int OptimizeSystem::checkLOD(glm::vec3 position, std::vector<LODLevel> LODs)
{
	size_t i = 0;
	for (; i < LODs.size(); i++)
	{
		if (glm::distance(camera->Position, position) < LODs[i].distance)
		{
			return i;
		}
	}

	return i - 1;
}

bool within(float min, float val, float max) {
    return val >= min && val <= max;
}

bool testAABBinFrustum(glm::mat4& MVP, const AABB& aabb)
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
            within(-corner.w, corner.x, corner.w) &&
                within(-corner.w, corner.y, corner.w) &&
                within(0.0f, corner.z, corner.w);
    }

    return inside;
}
 
void OptimizeSystem::objectsInFrustum(
    const std::shared_ptr<Camera>& camera,
    const std::vector<glm::mat4>& models,
    const std::vector<AABB>& aabbList,
    std::vector<unsigned int>& outVisibleList)
{
    glm::mat4 VP = camera->projection * camera->view;
    outVisibleList.clear();
    for (size_t i = 0; i < aabbList.size(); i++)
    {
        glm::mat4 MVP = VP * models[i]; 

        const AABB& aabb = aabbList[i];
        if (testAABBinFrustum(MVP, aabb))
        {
            outVisibleList.push_back(i);
        }
    }
}

