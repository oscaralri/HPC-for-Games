#pragma once

#include "Scene.h"
#include "Skybox.h"

class BaseScene : public Scene
{
public:
	void OnCreate() override
	{
		cameras["MainCamera"] = std::make_shared<Camera>(glm::vec3(20000.f, 5000.0f, 20000.f));
		cameras["ImguiCamera"] = std::make_shared<Camera>(glm::vec3(5.0f, 0.0f, 0.f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, -90.0f);
	}

	std::shared_ptr<Camera> GetCamera(const std::string& name)
	{
		auto it = this->cameras.find(name);
		if (it != this->cameras.end())
			return it->second;
		return nullptr;
	}

private:
};

