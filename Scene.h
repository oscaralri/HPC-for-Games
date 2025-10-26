#pragma once
#include <unordered_map>
#include <string>
#include <memory>

#include "Camera.h"

class Scene
{
private:
	std::unordered_map<std::string, std::shared_ptr<Camera>> cameras;

	Scene()
	{
		cameras["MainCamera"] = std::make_shared<Camera>(glm::vec3(5.0f, 0.0f, 5.0f));
		cameras["ImguiCamera"] = std::make_shared<Camera>(glm::vec3(0.0f, 10.0f, .0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, -90.0f);
	}

public:
	std::shared_ptr<Camera> getCamera(const std::string& name)
	{
		auto it = cameras.find(name);
		if (it != cameras.end())
			return it->second;
		return nullptr; 
	}
};

