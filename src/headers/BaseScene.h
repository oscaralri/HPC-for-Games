#pragma once

#include "Scene.h"
#include "Skybox.h"

class BaseScene : public Scene
{
public:
	void OnCreate() override
	{
		// no deberian hacer falta los this es mas por asegurar
		cameras["MainCamera"] = std::make_shared<Camera>(glm::vec3(5.0f, 0.0f, 5.0f));
		cameras["ImguiCamera"] = std::make_shared<Camera>(glm::vec3(0.0f, 10.0f, .0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, -90.0f);
	}

	std::shared_ptr<Camera> GetCamera(const std::string& name)
	{
		auto it = this->cameras.find(name);
		if (it != this->cameras.end())
			return it->second;
		return nullptr;
	}

private:
	//std::unordered_map<std::string, std::shared_ptr<Camera>> cameras;
	//std::shared_ptr<Skybox> skybox;


};

