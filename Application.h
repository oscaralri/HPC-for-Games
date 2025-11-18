#pragma once
#include "Scene.h"
#include "BaseScene.h"
#include "Renderer.h"
#include "RenderSystem.h"
#include "ECSConfig.h"
#include "Coordinator.h"
#include "Transform.h"
#include "Renderable.h"

extern ECS::Coordinator gCoordinator;

class Application
{
private:
	Scene* scene;
	
	Application() = default;                                
	Application(const Application&) = delete;               
	Application& operator=(const Application&) = delete;   

public:
	static Application& Get()
	{
		static Application instance;
		return instance;
	}

	void Init(); // definir e inicializar escena
	void Run(); // main Loop 
	void ECSInit();
	Scene* GetActiveScene() { return scene; }
};

