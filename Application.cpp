#include "Application.h"

void Application::Init()
{ 
	scene = new BaseScene();
	scene->OnCreate();
	
	Renderer::Get().Init();
}

void Application::Run()
{
	while (!glfwWindowShouldClose(Renderer::Get().GetWindow()))
	{
		Renderer::Get().Render();
	}

	Renderer::Get().End();
}