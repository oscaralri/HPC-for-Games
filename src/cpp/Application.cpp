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

	gCoordinator.RegisterComponent<Transform>();
	gCoordinator.RegisterComponent<Renderable>();
	gCoordinator.RegisterComponent<EntityMeshes>();
	gCoordinator.RegisterComponent<AABB>();
	
	// RENDER SYSTEM
	gCoordinator.RegisterSystem<RenderSystem>();
	{
		ECS::Signature signature;
		signature.set(gCoordinator.GetComponentType<Transform>());
		//signature.set(gCoordinator.GetComponentType<Renderable>());

		gCoordinator.SetSystemSignature<RenderSystem>(signature);
	}

	// MDI SYSTEM
	gCoordinator.RegisterSystem<MDI>();
	{
		ECS::Signature signature;
		signature.set(gCoordinator.GetComponentType<EntityMeshes>());
		
		gCoordinator.SetSystemSignature<MDI>(signature);
	}

	gCoordinator.RegisterSystem<CullingSystem>();
	{
		ECS::Signature signature;
		signature.set(gCoordinator.GetComponentType<Transform>());
		//signature.set(gCoordinator.GetComponentType<Renderable>());
		signature.set(gCoordinator.GetComponentType<AABB>());

		gCoordinator.SetSystemSignature<CullingSystem>(signature);
	}
}