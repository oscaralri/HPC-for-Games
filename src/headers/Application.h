#pragma once

#include "Scene.h"
#include "BaseScene.h"
#include "Renderer.h"

#include "ECSConfig.h"
#include "Coordinator.h"
#include "Transform.h"
#include "Renderable.h"
#include "Movement.h"

#include "RenderSystem.h"
#include "LODSystem.h"
#include "CullingSystem.h"
#include "BatchSystem.h"
#include "MovementSystem.h"

extern ECS::Coordinator gCoordinator;

class Application
{
public:
	static Application& Get()
	{
		static Application instance;
		return instance;
	}

	void Init(); 
	void Run(); // main Loop 
	
	Scene* GetActiveScene() { return scene; }

private:
	Scene* scene;
	void ECSInit();
	Application() = default;                                
	Application(const Application&) = delete;               
	Application& operator=(const Application&) = delete;   


};

