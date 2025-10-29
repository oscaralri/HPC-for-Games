#include "Application.h"

void Application::Init()
{
	/*
	scene = new BaseScene();
	scene->OnCreate();
	*/
	Scene* newScene = new BaseScene();
	newScene->OnCreate();
	scene = newScene;
	//auto scene = Application::Get().GetActiveScene();
	auto mainCamera = scene->GetCamera("MainCamera");
	auto currentScene = Application::Get().GetActiveScene();
	std::cout << "currentScene: " << currentScene << " mainCamera: " << mainCamera << " scene: " << scene;

	Renderer::Get().Init();
}

void Application::Run()
{
	Renderer& renderer = Renderer::Get();

	while (!glfwWindowShouldClose(renderer.GetWindow()))
	{
		renderer.Render();
	}
}