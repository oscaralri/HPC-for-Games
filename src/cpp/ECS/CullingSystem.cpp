#include "CullingSystem.h"

enum FrustumPlane
{
	LEFT_FRUSTUM, RIGHT_FRUSTUM, BOTTOM_FRUSTUM, TOP_FRUSTUM, NEAR_FRUSTUM, FAR_FRUSTUM
};

struct Plane
{
	glm::vec3 n;
	float d;
};

struct Frustum
{
	Plane planes[6];
};

Frustum CreateFrustum(glm::mat4 projection, glm::mat4 view, const std::shared_ptr<Camera>& camera)
{
	glm::mat4 projView = glm::transpose(projection * view);
	
	Frustum frustum;
	glm::vec4 aux;

	aux = projView[3] + projView[0];
	frustum.planes[LEFT_FRUSTUM] = Plane{ {aux.x, aux.y, aux.z}, aux.w };

	aux = projView[3] - projView[0];
	frustum.planes[RIGHT_FRUSTUM] = Plane{ {aux.x, aux.y, aux.z}, aux.w };

	aux = projView[3] + projView[1];
	frustum.planes[BOTTOM_FRUSTUM] = Plane{ {aux.x, aux.y, aux.z}, aux.w };

	aux = projView[3] - projView[1];
	frustum.planes[TOP_FRUSTUM] = Plane{ {aux.x, aux.y, aux.z}, aux.w };

	aux = projView[3] + projView[2];
	frustum.planes[NEAR_FRUSTUM] = Plane{ {aux.x, aux.y, aux.z}, aux.w };

	aux = projView[3] - projView[2];
	frustum.planes[FAR_FRUSTUM] = Plane{ {aux.x, aux.y, aux.z}, aux.w };

	// normalizar
	for (auto& plane : frustum.planes)
	{
		float len = glm::length(plane.n);
		plane.n /= len;
		plane.d /= len;
	}

	return frustum;
}

bool AABBIntersection(Frustum frustum, AABB aabb, Transform transform)
{
	glm::vec3 corners[8] =
	{
		{ aabb.min.x, aabb.min.y, aabb.min.z },
		{ aabb.max.x, aabb.min.y, aabb.min.z },
		{ aabb.min.x, aabb.max.y, aabb.min.z },
		{ aabb.max.x, aabb.max.y, aabb.min.z },
		{ aabb.min.x, aabb.min.y, aabb.max.z },
		{ aabb.max.x, aabb.min.y, aabb.max.z },
		{ aabb.min.x, aabb.max.y, aabb.max.z },
		{ aabb.max.x, aabb.max.y, aabb.max.z }
	};

	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, transform.position);
	model = glm::scale(model, transform.scale);

	glm::vec3 worldCorners[8];
	for (int i = 0; i < 8; i++)
	{
		worldCorners[i] = glm::vec3(model * glm::vec4(corners[i], 1.0f));
	}

	// cada plano
	for (size_t i = 0; i < 6; i++)
	{
		int out = 0;

		// cada punto
		for (size_t j = 0; j < 8; j++)
		{
			if (glm::dot(worldCorners[j], frustum.planes[i].n) + frustum.planes[i].d < 0.f)
				++out;
		}

		if (out == 8)
		{
			return false;
		}
	}

	return true;
}

std::vector<ECS::Entity> CullingSystem::FrustumCulling(ECS::Coordinator& coordinator, const std::shared_ptr<Camera>& camera, std::vector<GridCell> cells)
{
	std::vector<ECS::Entity> cellsVisible;
	std::vector<ECS::Entity> visibleList;
	std::vector<bool> entityVisited(mEntities.size(), false);
	Frustum frustum = CreateFrustum(camera->projection, camera->view, camera);
	
	for (auto const& cell : cells)
	{
		AABB aabb { cell.min, cell.max };

		Transform transform { glm::vec3(0.f,0.f,0.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(1.f, 1.f, 1.f)};

		if (AABBIntersection(frustum, aabb, transform))
		{
			for (auto const& entity : cell.entities)
			{
				cellsVisible.push_back(entity);
			}
		}
	}

	//std::cout << "celda activa: " << cell.min.x << " " << cell.min.y << " " << cell.min.z << " / " << cell.max.x << " " << cell.max.y << " " << cell.max.y << "\n" << std::endl;
	//std::cout << "----------------------------" << "\n" << std::endl;
	
	for (auto const& entity : cellsVisible)
	{
		auto& aabb = coordinator.GetComponent<AABB>(entity);
		auto& transform = coordinator.GetComponent<Transform>(entity);
		
		if (AABBIntersection(frustum, aabb, transform))
		{
			if (!entityVisited[entity])
			{
				entityVisited[entity] = true;
				visibleList.push_back(entity);
			}
			
		}
	}

	return visibleList;
}