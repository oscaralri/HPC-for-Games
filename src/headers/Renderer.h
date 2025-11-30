#pragma once

#include <iostream>
#include <random>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include <vector>

#include "Camera.h"
#include "Application.h"
#include "OptimizeSystem.h"
#include "GameObject.h"
#include "ShaderStorage.h"
#include "Skybox.h"
#include "CullingSystem.h"



class Renderer
{


public:
	static Renderer& Get()
	{
		static Renderer instance;
		return instance;
	}
	
	GLFWwindow* GetWindow() { return window; }

	void Init();
	void Render(); // main loop de render
	void End();

private:
	//test 
	std::vector<LODLevel> lods;
	std::shared_ptr<Model> gargoyle;

	//
	Renderer() = default;
	Renderer(const Renderer&) = delete;
	Renderer& operator=(const Renderer&) = delete;

	GLFWwindow* window;

	int SCR_WIDTH;
	int SCR_HEIGHT;
	float near;
	float far;
	std::shared_ptr<Camera> mainCamera;
	unsigned int cameraUBO;

	// frustum culling
	std::vector<glm::mat4> models;
	std::vector<AABB> aabb;
	std::vector<unsigned int> outList;

	std::vector<GameObject> gobjectsToRender;
	std::vector<GameObject> normalList;
	std::vector<GameObject> instancedList;

	unsigned int framebuffer;
	unsigned int textureColorbuffer;
	unsigned int quadVAO, quadVBO;
	unsigned int imguiFBO;

	//std::vector<Camera> cameras;
	//unsigned int mainCameraID;

	// time
	float deltaTime;
	float lastFrame;
	int nbFrames;
	double lastTime;
	double fps;

	bool moveEnabled;
	bool firstMouse;
	float lastX;
	float lastY;

	// Imgui
	unsigned int imguiTextureBuffer, imguiRBO;

	unsigned int buffer;

	std::vector<ECS::Entity> visibleInstanced;
	std::vector<ECS::Entity> visibleNormal;


	// methods
	void framebuffer_size_callback(GLFWwindow* window, int width, int height);
	void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
	void mouse_callback(GLFWwindow* window, double xpos, double ypos);
	void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
	void processInput(GLFWwindow* window);
	unsigned int loadTexture(const char* path);
	int WindowInit(int SCR_WIDTH, int SCR_HEIGHT);
	void FBOInit(int SCR_WIDTH, int SCR_HEIGHT);
	void ModelsInit();
	static void MouseCallbackDispatch(GLFWwindow* window, double xpos, double ypos);
	void GargoylesInstancing();
	void RenderNormal(std::vector<ECS::Entity> entities);
	void Instanced(std::vector<ECS::Entity> entities);
	void ShadersInit();
	void UpdateModelMat(std::vector<ECS::Entity>& entities, ECS::Coordinator& coordinator);
	void InitGargoylesECS();
	void SortRenderType(ECS::Coordinator& coordinator, std::vector<ECS::Entity> entities);

	// debug
	void showFPS(GLFWwindow* window);
};

