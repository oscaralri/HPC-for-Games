#include "Renderer.h"

#include "EngineResources.h"

void DebugAABB(glm::mat4 projection, glm::mat4 view, glm::vec3 min, glm::vec3 max)
{

	glm::vec3 corners[8] =
	{
		{min.x, min.y, min.z},
		{max.x, min.y, min.z},
		{max.x, max.y, min.z},
		{min.x, max.y, min.z},
		{min.x, min.y, max.z},
		{max.x, min.y, max.z},
		{max.x, max.y, max.z},
		{min.x, max.y, max.z}
	};

	Transform transform = {
		glm::vec3(0.f, 0.f, 0.f), // position 
		glm::vec3(0.f, 0.f, 0.f), // rotation 
		glm::vec3(1.f, 1.f, 1.f) // scale 
	};

	glm::mat4 model = glm::mat4(1.0f);
	model = glm::scale(model, transform.scale);
	model = glm::translate(model, transform.position);
	model = glm::rotate(model, glm::radians(transform.rotation.x), glm::vec3(1, 0, 0));
	model = glm::rotate(model, glm::radians(transform.rotation.y), glm::vec3(0, 1, 0));
	model = glm::rotate(model, glm::radians(transform.rotation.z), glm::vec3(0, 0, 1));

	glm::vec3 worldCorners[8];
	for (int i = 0; i < 8; i++)
	{
		worldCorners[i] = glm::vec3(model * glm::vec4(corners[i], 1.0f));
	}

	float lines[24 * 3];

	int idx = 0;
	auto addLine = [&](int a, int b) {
		lines[idx++] = worldCorners[a].x; lines[idx++] = worldCorners[a].y; lines[idx++] = worldCorners[a].z;
		lines[idx++] = worldCorners[b].x; lines[idx++] = worldCorners[b].y; lines[idx++] = worldCorners[b].z;
		};

	// Bottom face
	addLine(0, 1); addLine(1, 2); addLine(2, 3); addLine(3, 0);
	// Top face
	addLine(4, 5); addLine(5, 6); addLine(6, 7); addLine(7, 4);
	// Vertical edges
	addLine(0, 4); addLine(1, 5); addLine(2, 6); addLine(3, 7);

	GLuint VAO, VBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(lines), lines, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	auto rhs = EngineResources::GetShaderManager().LoadShader("shaders/plainColor.vert", "shaders/plainColor.frag");
	auto shader = EngineResources::GetShaderManager().Get(rhs);
	shader->use();
	shader->setMat4("projection", projection);
	shader->setMat4("view", view);
	shader->setMat4("model", model);

	glDrawArrays(GL_LINES, 0, 24);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glDeleteBuffers(1, &VBO);
	glDeleteVertexArrays(1, &VAO);

}

void Renderer::GenerateBatchEntity(ResourceHandle modelRH, ResourceHandle shader, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, int lodIncrement)
{
	//auto modelRH = EngineResources::GetModelManager().LoadModelLOD(modelPaths, lodIncrement);

	auto entity = gCoordinator.CreateEntity();
	gCoordinator.AddComponent(entity, Transform{
		position,
		rotation,
		scale
		});
	gCoordinator.AddComponent(entity, Renderable{ modelRH, shader, RenderType::Batch });
	gCoordinator.AddComponent(entity, AABB{
		EngineResources::GetModelManager().Get(modelRH)->getMinMax()[0],
		EngineResources::GetModelManager().Get(modelRH)->getMinMax()[1]
		});
	AABB& aabb = gCoordinator.GetComponent<AABB>(entity);
	Transform& transform = gCoordinator.GetComponent<Transform>(entity);

	glm::vec3 worldMin = transform.position + aabb.min * transform.scale;
	glm::vec3 worldMax = transform.position + aabb.max * transform.scale;
	grid->Insert(entity, worldMin, worldMax);
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
	std::shared_ptr<Skybox> newSkybox = std::make_shared<Skybox>(skyboxFaces, "shaders/skybox.vert", "shaders/skybox.frag");
	auto scene = Application::Get().GetActiveScene();
	scene->SetSkybox(newSkybox);
}

void TexturesInit()
{
	EngineResources::GetTextureManager().LoadTextures("textures/texturesJSON.json");
}

void Renderer::ShadersInit()
{
	screenShader = EngineResources::GetShaderManager().LoadShader("shaders/framebuffer_screen.vert", "shaders/framebuffer_screen.frag");
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

	// Uniform Buffer
	glGenBuffers(1, &cameraUBO);
	glBindBuffer(GL_UNIFORM_BUFFER, cameraUBO);
	glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), nullptr, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, 3, cameraUBO);
}

void Renderer::ModelsInit()
{
	// Grid(origin, worldSize, cellSize)
	grid = std::make_unique<Grid>(glm::vec3(-250.f, -250.f, -1000.f), glm::vec3(1000.f), glm::vec3(500.f));
	
	RandomGenerator random(ECS::MAX_ENTITIES, 123, -500, 500, 0, 0, -300, 0);

	// BATCHING
	batchingShader = EngineResources::GetShaderManager().LoadShader("shaders/batching.vert", "shaders/batching.frag");
	// gargoyle
	{
		std::vector<std::string> path = { "models/gargoyle/gargoyle.obj", "models/gargoyle/gargoyleLOW.obj" };
		auto shader = EngineResources::GetShaderManager().LoadShader("shaders/a_buffers.vert", "shaders/a_buffers.frag");
		auto modelRH = EngineResources::GetModelManager().LoadModelLOD(path, 25);

		for (int i = 0; i < 25; i++)
		{
			GenerateBatchEntity(modelRH, shader, glm::vec3((i + 10.f) * 1.5f, (i) * -1.5f, -250.f), glm::vec3(0.f, 180.f, 0.f), glm::vec3(0.5f), 25);
		}
		/*
		std::vector<std::string> path2 = { "models/chair/Pipo_chair_fix.fbx", "models/gargoyle/gargoyleLOW.obj" };
		auto modelRH2 = EngineResources::GetModelManager().LoadModelLOD(path2, 25);
		for (int i = 0; i < 200; i++)
		{
			GenerateBatchEntity(modelRH2, shader, glm::vec3(i * 1.5f, (i) * 1.5f, -250.f), glm::vec3(0.f), glm::vec3(0.5f), 25);
		}
		*/
	}

	// ENTITIES TO GRID
	for (auto& cell : grid->cells)
	{
		auto batchSystem = gCoordinator.GetSystem<BatchSystem>();
		batchSystem->BuildCellBatches(cell, cell.entities, gCoordinator);
	}
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
	SCR_WIDTH = 1024; // porta: 1024 x 576, PC: 1366x768 
	SCR_HEIGHT = 576;
	near = 0.1f;
	far = 2000.f;
	deltaTime = 0.0f;
	lastFrame = 0.0f;
	nbFrames = 0;
	lastTime = 0.;
	fps = 0.;
	moveEnabled = true;
	firstMouse = true;
	firstMouse = true;
	lastX = SCR_WIDTH / 2.0f;
	lastY = SCR_HEIGHT / 2.0f;
	isImgui = false;
	imguiCamPosX = -16.000;
	imguiCamPosY = 75.f;
	imguiCamPosZ = -10.f;

	buffers = std::vector<unsigned int>(ECS::MAX_ENTITIES);

	WindowInit(SCR_WIDTH, SCR_HEIGHT); // glfw and glad
	ShadersInit();
	TexturesInit();
	ModelsInit();
	FBOInit(SCR_WIDTH, SCR_HEIGHT);
	ImGuiInit(window);
	SkyboxInit();			
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

	glBindBuffer(GL_UNIFORM_BUFFER, cameraUBO);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(view));
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(projection));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	// FRUSTUM
	auto cullingSystem = gCoordinator.GetSystem<CullingSystem>();
	std::vector<GridCell> visibleCells = cullingSystem->FrustumCulling(gCoordinator, mainCamera, grid->cells);

	std::vector<ECS::Entity> visibleEntities;
	for (const auto& cells : visibleCells)
	{
		for (const auto& entity : cells.entities)
		{
			visibleEntities.push_back(entity);
		}
	}

	// FRAMEBUFFER
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDisable(GL_DEPTH_TEST);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	// INPUT
	float currentFrame = static_cast<float>(glfwGetTime());
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;

	processInput(window);

	// RENDER TO FBO
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.f, 0.f, 0.f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	showFPS(window);

	// SKYBOX
	glm::mat4 skyboxView = glm::mat4(glm::mat3(mainCamera->GetViewMatrix()));
	scene->GetSkybox()->Draw(mainCamera->projection, skyboxView);

	// BATCHING
	RenderBatching(visibleCells, mainCamera);

	// RENDERIZAR TO IMGUI
	RenderImGUI(visibleCells);
	RenderImGUICamera(imguiCamera, projection, view);

	// BACK TO DEFAULT FBO
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDisable(GL_DEPTH_TEST);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	// FULLSCREEN QUAD DRAW
	EngineResources::GetShaderManager().Get(screenShader)->use();
	glBindVertexArray(quadVAO);
	glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	glfwSwapBuffers(window);
	glfwPollEvents();
}

void Renderer::RenderImGUICamera(std::shared_ptr<Camera> imguiCamera, glm::mat4 projection, glm::mat4 view)
{
	if (isImgui)
	{
		imguiCamera->Position = glm::vec3(imguiCamPosX, imguiCamPosY, imguiCamPosZ);

		glBindFramebuffer(GL_FRAMEBUFFER, imguiFBO);
		glEnable(GL_DEPTH_TEST);
		glClearColor(0.f, 0.f, 0.f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 imguiProj = glm::perspective(glm::radians(imguiCamera->Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, near, far);
		glm::mat4 imguiView = glm::lookAt(imguiCamera->Position, imguiCamera->Position + imguiCamera->Front, imguiCamera->Up);

		glBindBuffer(GL_UNIFORM_BUFFER, cameraUBO);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(imguiView));
		glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(imguiProj));
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		// draw on imgui
		ImGui::Begin("Render Debug");
		ImGui::Checkbox("DebugAABB", &isDebugAABB);
		ImGui::Checkbox("ImguiCamera", &isImgui);
		ImGui::Image((ImTextureID)(intptr_t)imguiTextureBuffer, ImVec2(SCR_WIDTH / 3, SCR_HEIGHT / 3), ImVec2(0, 1), ImVec2(1, 0));
		ImGui::DragFloat("X", &imguiCamPosX, 0.5f);
		ImGui::DragFloat("Y", &imguiCamPosY, 0.5f);
		ImGui::DragFloat("Z", &imguiCamPosZ, 0.5f);
		ImGui::End();
	}
	else
	{
		ImGui::Begin("Render Debug");
		ImGui::Checkbox("DebugAABB", &isDebugAABB);
		ImGui::Checkbox("ImguiCamera", &isImgui);
		ImGui::End();
	}

	if (isDebugAABB)
	{
		for (const auto& cell : grid->cells)
		{
			DebugAABB(projection, view, cell.min, cell.max);
		}
	}
}

void Renderer::RenderImGUI(std::vector<GridCell> visibleCells)
{
	ImGui::Begin("Render Info");
	float wrapWidth = ImGui::GetWindowContentRegionMax().x;
	ImGui::PushTextWrapPos(wrapWidth);
	ImGui::Text("Outlist (VAO ids):");
	std::string str;
	for (size_t i = 0; i < visibleBatching.size(); ++i) {
		str += std::to_string(visibleBatching[i].vao);
		if (i != visibleBatching.size() - 1)
			str += ", ";
	}
	ImGui::Text("%s", str.c_str());
	ImGui::Text("Num visibleCells:");
	str = std::to_string(visibleCells.size());
	ImGui::Text("%s", str.c_str());
	ImGui::PopTextWrapPos();
	ImGui::End();
}

void Renderer::RenderBatching(std::vector<GridCell>& cells, std::shared_ptr<Camera> camera)
{
	visibleBatching.clear();
	int lodLevel = 0;
	for (const auto& cell : cells)
	{
		auto lodSystem = gCoordinator.GetSystem<LODSystem>();
		lodLevel = lodSystem->SetLODFromAABB(cell, camera->Position);
		for (auto& batch : cell.lodBatches[lodLevel])
		{
			visibleBatching.push_back(batch);
		}
	}

	std::sort(visibleBatching.begin(), visibleBatching.end(), [](StaticBatch a, StaticBatch b) {
		return a.shader < b.shader;
		});

	auto renderSystem = gCoordinator.GetSystem<RenderSystem>();
	renderSystem->RenderBatch(visibleBatching);;
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
