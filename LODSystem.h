#pragma once
#include "Camera.h"
#include "Model.h"
#include <vector>
#include <iostream>

// se esta inicializando con un valor por ahora, tendra que cambiarse
struct AABB
{
	glm::vec3 min = { 15.f, -10.f, -10.f };
	glm::vec3 max = { 20.f, 10.f, 10.f };
};

class LODSystem
{
private:
	Camera* camera;
	
	LODSystem() 
	{
	}
	LODSystem(const LODSystem&) = delete;
	LODSystem& operator=(const LODSystem&) = delete;

public:
	static LODSystem& getInstance() 
	{
		static LODSystem instance; 
		return instance;
	}

	void setCamera(Camera* cam) 
	{
		camera = cam;
	}

	int checkLOD(glm::vec3 position, std::vector<LODLevel>);


	// esto son cosas del frustum culling que no deberian ir aqui pero por ahora
	void objectsInFrustum(const Camera& camera,
		const std::vector<glm::vec3>& transforms,
		const std::vector<AABB>& aabbList,
		std::vector<unsigned int>& outList);
};

