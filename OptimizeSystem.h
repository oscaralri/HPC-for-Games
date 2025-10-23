#pragma once
#include "Camera.h"
#include "Model.h"
#include <vector>
#include <iostream>

struct AABB
{
	glm::vec3 min;
	glm::vec3 max;
};

class OptimizeSystem
{
private:
	Camera* camera;
	
	OptimizeSystem() {}
	OptimizeSystem(const OptimizeSystem&) = delete;
	OptimizeSystem& operator=(const OptimizeSystem&) = delete;

public:
	static OptimizeSystem& getInstance() 
	{
		static OptimizeSystem instance; 
		return instance;
	}

	void setCamera(Camera* cam) { camera = cam;	}

	int checkLOD(glm::vec3 position, std::vector<LODLevel>);

	void objectsInFrustum(const Camera& camera,
		const std::vector<glm::mat4>& models,
		const std::vector<AABB>& aabbList,
		std::vector<unsigned int>& outList);

	void findAABBMinMax(const std::vector<Mesh> meshes, glm::vec3& aabbMin, glm::vec3& aabbMax);
};

