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
#include "ShaderStorage.h"
#include "Skybox.h"
#include "CullingSystem.h"
#include "Grid.h"
#include "RandomGenerator.h"
#include "MDI.h"


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
	Renderer() = default;
	Renderer(const Renderer&) = delete;
	Renderer& operator=(const Renderer&) = delete;

	GLFWwindow* window;

	int SCR_WIDTH;
	int SCR_HEIGHT;
	float near;
	float far;
	std::shared_ptr<Camera> mainCamera;
	std::shared_ptr<Camera> imguiCamera;
	GLuint cameraUBO;
	GLuint frustumUBO;

	Scene* scene;

	//ImGUI
	bool isImgui;
	bool isDebugGrid;
	float imguiCamPosX;
	float imguiCamPosY;
	float imguiCamPosZ;
	unsigned int imguiTextureBuffer, imguiRBO;

	// shaders 
	ResourceHandle screenShader;
	ResourceHandle renderShader;
	ResourceHandle computeShader;

	unsigned int framebuffer;
	unsigned int textureColorbuffer;
	unsigned int quadVAO, quadVBO;
	unsigned int imguiFBO;

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


	std::unique_ptr<Grid> grid;

	// methods
	void framebuffer_size_callback(GLFWwindow* window, int width, int height);
	void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
	void mouse_callback(GLFWwindow* window, double xpos, double ypos);
	void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
	void processInput(GLFWwindow* window);
	int WindowInit(int SCR_WIDTH, int SCR_HEIGHT);
	void FBOInit(int SCR_WIDTH, int SCR_HEIGHT);
	void ModelsInit();
	void ShadersInit();

	void RenderImGUI();
	void RenderImGUICamera(std::shared_ptr<Camera> imguiCamera, glm::mat4 projection, glm::mat4 view, Shader* computeS, Shader* renderS);

	void GenerateMDIEntity(ResourceHandle modelRH, EntityMeshes& entityMeshes, glm::vec3 position, glm::vec3 scale);
	void GenerateMDIEntityRandom(ResourceHandle modelRH, MeshEntry& mesh, RandomGenerator& random);

	// debug
	void showFPS(GLFWwindow* window);
};

