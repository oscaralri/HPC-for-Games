#include "CullingSystem.h"

// new
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
	glm::mat4 projView = projection * view;
	
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

	// asegurar direccion hacia dentro
	for (auto& plane : frustum.planes)
	{
		float dist = glm::dot(plane.n, camera->Position) + plane.d;
		if (dist < 0.0f)
		{
			plane.n = -plane.n;
			plane.d = -plane.d;
		}
	}

	// normalizar
	for (auto& plane : frustum.planes)
	{
		float len = glm::length(plane.n);
		plane.n /= len;
		plane.d /= len;

		std::cout << plane.n.x << " " << plane.n.y << " " << plane.n.z << std::endl;
	}

	std::cout << "\n";

	return frustum;
}

bool AABBIntersection(Frustum frustum, AABB aabb)
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

	Transform transform = {
		glm::vec3(0.f, 0.f, 0.f), // position 
		glm::vec3(0.f, 0.f, 0.f), // rotation 
		glm::vec3(1.f, 1.f, 1.f) // scale 
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
			if (glm::dot(worldCorners[j], frustum.planes[i].n) + frustum.planes[i].d > 0.f)
				++out;
		}

		if (out == 8)
		{
			return false;
		}
	}

	return true;
}

std::vector<ECS::Entity> CullingSystem::NewFrustumCulling(ECS::Coordinator& coordinator, const std::shared_ptr<Camera>& camera)
{
	std::vector<ECS::Entity> visibleList;
	Frustum frustum = CreateFrustum(camera->projection, camera->view, camera);

	for (auto const& entity : mEntities)
	{
		//auto& aabb = coordinator.GetComponent<AABB>(entity);
		
		AABB test = {
			glm::vec3(0.f, 0.f, 0.f),
			glm::vec3(5.f, 5.f, 5.0f)
		};
		
		if (AABBIntersection(frustum, test))
		{
			visibleList.push_back(entity);
		}
	}

	return visibleList;
}


// old
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
		auto& transform = coordinator.GetComponent<Transform>(entity);
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



