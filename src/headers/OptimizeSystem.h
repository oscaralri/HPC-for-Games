#pragma once
#include "Camera.h"
#include "Model.h"
#include <vector>
#include <iostream>
#include <AABB.h>

class OptimizeSystem
{


public:
	// LACAMARAAA pPRIVATOOOOOOOOOOOOO
	std::shared_ptr<Camera> camera;

	static OptimizeSystem& getInstance() 
	{
		static OptimizeSystem instance; 
		return instance;
	}

	void setCamera(std::shared_ptr<Camera> cam) { camera = cam;	}

	int checkLOD(glm::vec3 position, std::vector<LODLevel>);

	void objectsInFrustum(const std::shared_ptr<Camera>& camera,
		const std::vector<glm::mat4>& models,
		const std::vector<AABB>& aabbList,
		std::vector<unsigned int>& outList);

	void findAABBMinMax(const std::vector<Mesh> meshes, glm::vec3& aabbMin, glm::vec3& aabbMax);

private:

	OptimizeSystem() {}
	OptimizeSystem(const OptimizeSystem&) = delete;
	OptimizeSystem& operator=(const OptimizeSystem&) = delete;
};

