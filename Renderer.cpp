#include "Renderer.h"


void SeedInit()
{
	unsigned int seed = 12345;
	std::mt19937 rng(seed);
	std::cout << "" << std::endl;
	std::cout << rng << std::endl;
}

void GargoylesInit(std::vector<GameObject>& gobjectsToRender, std::vector<glm::mat4>& models, std::vector<AABB>& aabb)
{
	std::vector<std::string> paths = { "models/gargoyle/gargoyle.obj", "models/gargoyle/gargoyleLOW.obj" };
	auto gargoyle = std::make_shared<Model>(paths, 25);

	glm::vec3 basePosition(5.f, -2.f, -5.f);
	glm::vec3 rotation = glm::vec3(0.f, 180.f, 0.f);
	glm::vec3 scale = glm::vec3(0.045f, 0.045, 0.045);

	auto modelLoading = ShaderStorage::Get().GetShader("modelLoading");

	for (int i = 0; i < 20; i++)
	{
		glm::vec3 pos = basePosition + glm::vec3(0.f, 0.f, -i * 5.f);
		gobjectsToRender.emplace_back(i, gargoyle, *modelLoading, pos, rotation, scale);
	}

	for (size_t i = 0; i < gobjectsToRender.size(); i++)
	{
		models.push_back(gobjectsToRender[i].getModelMatrix());
		aabb.push_back(gobjectsToRender[i].getAABB());
	}
}

void SkyboxInit()
{
	std::vector<std::string> skyboxFaces = {
		"textures/skybox/right.jpg",
		"textures/skybox/left.jpg",
		"textures/skybox/top.jpg",
		"textures/skybox/bottom.jpg",
		"textures/skybox/front.jpg",
		"textures/skybox/back.jpg"
	};
	std::shared_ptr<Skybox> newSkybox = std::make_shared<Skybox>(skyboxFaces);
	auto scene = Application::Get().GetActiveScene();
	scene->SetSkybox(newSkybox);
}

void ShadersInit()
{
	auto modelLoading = std::make_shared<Shader>("shaders/modelLoading.vert", "shaders/modelLoading.frag");
	ShaderStorage::Get().Add("modelLoading", modelLoading);

	auto screenShader = std::make_shared<Shader>("shaders/framebuffer_screen.vert", "shaders/framebuffer_screen.frag");
	ShaderStorage::Get().Add("screenShader", screenShader);

	auto skyboxShader = std::make_shared<Shader>("shaders/skybox.vert", "shaders/skybox.frag");
	ShaderStorage::Get().Add("skyboxShader", skyboxShader);
}

void ImGuiInit(GLFWwindow* window)
{
	// Initialize ImGUI
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls

	// Setup Platform/Renderer backends	
	ImGui_ImplGlfw_InitForOpenGL(window, true); // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
	ImGui_ImplOpenGL3_Init();
}

void Renderer::FBOInit(int SCR_WIDTH, int SCR_HEIGHT)
{
	// FBO
	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

	// almacenar color
	glGenTextures(1, &textureColorbuffer);
	glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);

	// almacenar profundidad y stencil
	unsigned int rbo;
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCR_WIDTH, SCR_HEIGHT);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// imgui FBO
	glGenFramebuffers(1, &imguiFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, imguiFBO);

	glGenTextures(1, &imguiTextureBuffer);
	glBindTexture(GL_TEXTURE_2D, imguiTextureBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, imguiTextureBuffer, 0);

	// no estoy seguro si esto aqui hace falta
	glGenRenderbuffers(1, &imguiRBO);
	glBindRenderbuffer(GL_RENDERBUFFER, imguiRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCR_WIDTH, SCR_HEIGHT);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, imguiRBO);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Quad
	float quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
		// positions   // texCoords
		-1.0f,  1.0f,  0.0f, 1.0f,
		-1.0f, -1.0f,  0.0f, 0.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,

		-1.0f,  1.0f,  0.0f, 1.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,
		 1.0f,  1.0f,  1.0f, 1.0f
	};

	glGenVertexArrays(1, &quadVAO);
	glGenBuffers(1, &quadVBO);
	glBindVertexArray(quadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
}

void Renderer::ModelsInit()
{
	GargoylesInit(gobjectsToRender, models, aabb);	
}

int Renderer::WindowInit(int SCR_WIDTH, int SCR_HEIGHT)
{
	// glfw
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "TFG", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// input 
	glfwSetWindowUserPointer(window, this);

	glfwSetCursorPosCallback(window, [](GLFWwindow* w, double xpos, double ypos)
		{
			Renderer* r = static_cast<Renderer*>(glfwGetWindowUserPointer(w));
			if (r) r->mouse_callback(w, xpos, ypos); 
		});

	glfwSetScrollCallback(window, [](GLFWwindow* w, double xoffset, double yoffset)
		{
			Renderer* r = static_cast<Renderer*>(glfwGetWindowUserPointer(w));
			if (r) r->scroll_callback(w, xoffset, yoffset);
		});
	glfwSetKeyCallback(window, [](GLFWwindow* w, int key, int scancode, int action, int mods)
		{			
			Renderer* r = static_cast<Renderer*>(glfwGetWindowUserPointer(w));
			if (r) r->key_callback(w, key, scancode, action, mods);
		});
	
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // GLFW_CURSOR_DISABLED GLFW_CURSOR_HIDDEN GLFW_CURSOR_NORMAL
	
	// glad
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

	glfwSetFramebufferSizeCallback(window, [](GLFWwindow* w, int width, int height)
		{
			Renderer* r = static_cast<Renderer*>(glfwGetWindowUserPointer(w));
			if (r) r->framebuffer_size_callback(w, width, height);
		});

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	glFrontFace(GL_CW);
}

void Renderer::showFPS(GLFWwindow* window) {
	double currentTime = glfwGetTime();
	nbFrames++;
	
	if (currentTime - lastTime >= 1.0)
	{
		fps = double(nbFrames) / (currentTime - lastTime);
		nbFrames = 0;
		lastTime = currentTime;
	}

	ImGui::Begin("Performance");
	ImGui::Text("FPS: %.1f", fps);
	ImGui::Text("Frame time: %.2f ms", 1000.0 / fps);
	ImGui::End();
}

void Renderer::Init()
{
	SCR_WIDTH = 1366;
	SCR_HEIGHT = 768;
	near = 0.1f;
	far = 1000.f;
	deltaTime = 0.0f;
	lastFrame = 0.0f;
	nbFrames = 0;
	lastTime = 0.;
	fps = 0.;
	moveEnabled = true;
	firstMouse = true;
	lastX = SCR_WIDTH / 2.0f;
	lastY = SCR_HEIGHT / 2.0f;

	WindowInit(SCR_WIDTH, SCR_HEIGHT); // glfw and glad
	ShadersInit();
	ModelsInit();
	FBOInit(SCR_WIDTH, SCR_HEIGHT);
	ImGuiInit(window);
	SkyboxInit();
	SeedInit();

	OptimizeSystem::getInstance().setCamera(mainCamera);
}

void Renderer::Render()
{
	auto scene = Application::Get().GetActiveScene();
	mainCamera = scene->GetCamera("MainCamera");
	auto imguiCamera = scene->GetCamera("ImguiCamera");

	glm::mat4 projection = glm::perspective(glm::radians(mainCamera->Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, near, far);
	glm::mat4 view = glm::lookAt(mainCamera->Position, mainCamera->Position + mainCamera->Front, mainCamera->Up);
	mainCamera->projection = projection;
	mainCamera->view = view;

	// para poder cambiar la posicion en imgui
	static float posX = -16.000;
	static float posY = 75.f;
	static float posZ = -10.f;
	imguiCamera->Position = glm::vec3(posX, posY, posZ);

	OptimizeSystem::getInstance().objectsInFrustum(mainCamera, models, aabb, outList);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDisable(GL_DEPTH_TEST);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	// IMGUI
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	float currentFrame = static_cast<float>(glfwGetTime());
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;

	processInput(window);

	// renderizar escena a fbo
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.f, 0.f, 0.f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	showFPS(window);

	// projection / view
	glm::mat4 skyboxView = glm::mat4(glm::mat3(mainCamera->GetViewMatrix()));

	// Skybox	
	scene->GetSkybox()->Draw(mainCamera->projection, skyboxView);

	auto modelLoading = ShaderStorage::Get().GetShader("modelLoading");
	modelLoading->use();
	modelLoading->setMat4("projection", projection);
	modelLoading->setMat4("view", view);
	
	for (auto index : outList)
	{
		gobjectsToRender[index].Render();
	}

	ImGui::Begin("OutList");
	ImGui::Text("outlist:");
	std::string str;
	for (size_t i = 0; i < outList.size(); ++i) {
		str += std::to_string(outList[i]);
		if (i != outList.size() - 1)
			str += ", ";
	}
	ImGui::Text("%s", str.c_str());
	ImGui::End();

	// RENDERIZAR A IMGUI
	glBindFramebuffer(GL_FRAMEBUFFER, imguiFBO);
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.f, 0.f, 0.f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// projection / view
	projection = glm::perspective(glm::radians(imguiCamera->Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, near, far);
	view = glm::lookAt(imguiCamera->Position, imguiCamera->Position + imguiCamera->Front, imguiCamera->Up);

	modelLoading->use();
	modelLoading->setMat4("projection", projection);
	modelLoading->setMat4("view", view);
	
	for (auto index : outList)
	{
		gobjectsToRender[index].Render();
	}
	
	// volver a framebuffer principal
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDisable(GL_DEPTH_TEST);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	// dibujar fbo imgui
	ImGui::Begin("TopDown");
	ImGui::Image((ImTextureID)(intptr_t)imguiTextureBuffer, ImVec2(SCR_WIDTH / 3, SCR_HEIGHT / 3), ImVec2(0, 1), ImVec2(1, 0));
	ImGui::DragFloat("X", &posX, 0.5f);
	ImGui::DragFloat("Y", &posY, 0.5f);
	ImGui::DragFloat("Z", &posZ, 0.5f);
	ImGui::End();

	// dibujar un quad para pintarlo en toda la pantalla
	auto screenShader = ShaderStorage::Get().GetShader("screenShader");
	screenShader->use();
	glBindVertexArray(quadVAO);
	glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	glfwSwapBuffers(window);
	glfwPollEvents();
}

void Renderer::End()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	Application::Get().GetActiveScene()->GetSkybox()->Delete();

	glfwTerminate();
}

void Renderer::framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void Renderer::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_LEFT_CONTROL && action == GLFW_PRESS)
	{
		int cursorState = glfwGetInputMode(window, GLFW_CURSOR);

		if (cursorState == GLFW_CURSOR_DISABLED)
		{
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}
		else if (cursorState == GLFW_CURSOR_NORMAL)
		{
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}

		moveEnabled = !moveEnabled;
	}
}

void Renderer::mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
	if (!moveEnabled) return;

	float xpos = static_cast<float>(xposIn);
	float ypos = static_cast<float>(yposIn);

	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;

	lastX = xpos;
	lastY = ypos;

	mainCamera->ProcessMouseMovement(xoffset, yoffset);
}

void Renderer::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	mainCamera->ProcessMouseScroll(static_cast<float>(yoffset));
}

void Renderer::processInput(GLFWwindow* window)
{
	if (!moveEnabled) return;

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		mainCamera->ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		mainCamera->ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		mainCamera->ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		mainCamera->ProcessKeyboard(RIGHT, deltaTime);


}

unsigned int Renderer::loadTexture(char const* path)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}

