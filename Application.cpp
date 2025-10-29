#include "Application.h"

void Application::Init()
{ 
	scene = new BaseScene();
	scene->OnCreate();
	
	Renderer::Get().Init();
}

void Application::Run()
{
	Renderer& renderer = Renderer::Get();

	while (!glfwWindowShouldClose(renderer.GetWindow()))
	{
		Renderer::Get().Render();
	}
}