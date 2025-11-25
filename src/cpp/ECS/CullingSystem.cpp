#include "CullingSystem.h"

bool Within(float min, float val, float max)
{
	return val >= min && val <= max;
}

bool TestAABBinFrustum(glm::mat4& MVP, const AABB& aabb)
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
			Within(-corner.w, corner.x, corner.w) &&
			Within(-corner.w, corner.y, corner.w) &&
			Within(0.0f, corner.z, corner.w);
	}

	return inside;
}

std::vector<ECS::Entity> CullingSystem::FrustumCulling(ECS::Coordinator& coordinator, const std::shared_ptr<Camera>& camera)
{
	glm::mat4 VP = camera->projection * camera->view;
	std::vector<ECS::Entity> visibleList;

	for (auto const& entity : mEntities)
	{
		auto& transform = coordinator.GetComponent<TransformECS>(entity);
		auto& renderable = coordinator.GetComponent<Renderable>(entity);
		auto& aabb = coordinator.GetComponent<AABB>(entity);

		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, transform.position);
		model = glm::scale(model, transform.scale);
		model = glm::rotate(model, glm::radians(transform.rotation.x), glm::vec3(1, 0, 0));
		model = glm::rotate(model, glm::radians(transform.rotation.y), glm::vec3(0, 1, 0));
		model = glm::rotate(model, glm::radians(transform.rotation.z), glm::vec3(0, 0, 1));

		glm::mat4 MVP = VP * model;
		
		if (TestAABBinFrustum(MVP, aabb))
		{
			visibleList.push_back(entity);
		}
	}

	return visibleList;
}



