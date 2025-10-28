#pragma once
#include "Scene.h"
#include "BaseScene.h"
#include "Renderer.h"

class Application
{
private:
	Scene* scene;
	// Renderer* renderer;
	
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
	Scene* GetActiveScene() { return scene; }
};

