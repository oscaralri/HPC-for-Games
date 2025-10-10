#pragma once
#include "Camera.h"
#include <iostream>

class LODSystem
{
private:
	Camera camera;
	
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

	void setCamera(const Camera& cam) 
	{
		camera = cam;
	}

	int checkLOD(glm::vec3 position);
};

