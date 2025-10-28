#pragma once

#include "Scene.h"
#include "Skybox.h"

class BaseScene : public Scene
{
private:
	std::unordered_map<std::string, std::shared_ptr<Camera>> cameras;
	std::vector<std::string> skyboxFaces = {
		"textures/skybox/right.jpg",
		"textures/skybox/left.jpg",
		"textures/skybox/top.jpg",
		"textures/skybox/bottom.jpg",
		"textures/skybox/front.jpg",
		"textures/skybox/back.jpg"
	};
	std::shared_ptr<Skybox> skybox;

public:
	void OnCreate() override
	{
		cameras["MainCamera"] = std::make_shared<Camera>(glm::vec3(5.0f, 0.0f, 5.0f));
		cameras["ImguiCamera"] = std::make_shared<Camera>(glm::vec3(0.0f, 10.0f, .0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, -90.0f);

		skybox = std::make_shared<Skybox>(skyboxFaces);
	}
};

