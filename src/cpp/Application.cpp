#include "Application.h"


ECS::Coordinator gCoordinator;


void Application::Init()
{ 
	scene = new BaseScene();
	scene->OnCreate();
	
	ECSInit(); 

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

void Application::ECSInit()
{
	gCoordinator.Init();

	// RENDER SYSTEM
	gCoordinator.RegisterComponent<TransformECS>();
	gCoordinator.RegisterComponent<Renderable>();
	
	gCoordinator.RegisterSystem<RenderSystem>();
	{
		ECS::Signature signature;
		signature.set(gCoordinator.GetComponentType<TransformECS>());
		signature.set(gCoordinator.GetComponentType<Renderable>());

		gCoordinator.SetSystemSignature<RenderSystem>(signature);
	}

	// CULLING SYSTEM
	gCoordinator.RegisterComponent<AABB>();

	gCoordinator.RegisterSystem<CullingSystem>();
	{
		ECS::Signature signature;
		signature.set(gCoordinator.GetComponentType<TransformECS>());
		signature.set(gCoordinator.GetComponentType<Renderable>());
		signature.set(gCoordinator.GetComponentType<AABB>());

		gCoordinator.SetSystemSignature<CullingSystem>(signature);
	}
	
}