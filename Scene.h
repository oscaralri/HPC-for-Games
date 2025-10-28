#pragma once
#include <unordered_map>
#include <string>
#include <memory>

#include "Camera.h"

class Scene
{
private:
	std::unordered_map<std::string, std::shared_ptr<Camera>> cameras;
	std::shared_ptr<Skybox> skybox;

public:
	virtual ~Scene() = default;
	virtual void OnCreate() {}

	std::unordered_map<std::string, std::shared_ptr<Camera>> GetAllCameras() { return cameras; }

	std::shared_ptr<Camera> GetCamera(const std::string& name)
	{
		auto it = cameras.find(name);
		if (it != cameras.end())
			return it->second;
		return nullptr; 
	}

	std::shared_ptr<Skybox> GetSkybox() { return skybox; }
};

