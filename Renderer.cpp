#include "Renderer.h"

int Renderer::WindowInit(int SCR_WIDTH, int SCR_HEIGHT)
{
	// glfw
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "TFG", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// input 
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetKeyCallback(window, key_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // GLFW_CURSOR_DISABLED GLFW_CURSOR_HIDDEN GLFW_CURSOR_NORMAL

	// glad
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
}

void showFPS(GLFWwindow* window) {
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

int Renderer::Init()
{
	SCR_WIDTH;
	SCR_HEIGHT;
	near;
	far;

	WindowInit(SCR_WIDTH, SCR_HEIGHT); // glfw and glad
	
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	glFrontFace(GL_CW);
}

void Renderer::Render()
{
	glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, near, far);
	glm::mat4 view = glm::lookAt(camera.Position, camera.Position + camera.Front, camera.Up);
	camera.projection = projection;
	camera.view = view;

	imguiCamera.Position = glm::vec3(posX, posY, posZ);

	OptimizeSystem::getInstance().objectsInFrustum(camera, models, aabb, outList);

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
	glm::mat4 skyboxView = glm::lookAt(camera.Position, camera.Position + camera.Front, camera.Up);
	glm::mat4 model;


	// Cubo AABB test
	plainColor.use();
	plainColor.setMat4("projection", projection);
	plainColor.setMat4("view", view);
	model = glm::mat4(1.0f);
	plainColor.setMat4("model", model);
	glm::mat4 MVP = camera.projection * camera.view * model;
	glm::vec4 transformedCorners[8];

	/*
	for (int i = 0; i < 8; i++) {
		transformedCorners[i] = gobjectsToRender[0].getModelMatrix() * corners[i];
	}
	DrawAABB(transformedCorners, plainColor);
	*/

	// Skybox	
	glDepthFunc(GL_LEQUAL);
	skyboxShader.use();

	skyboxView = glm::mat4(glm::mat3(camera.GetViewMatrix())); // no translation
	skyboxShader.setMat4("view", skyboxView);
	skyboxShader.setMat4("projection", projection);
	glBindVertexArray(skyboxVAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glDepthFunc(GL_LESS); // depth default

	modelLoading.use();
	modelLoading.setMat4("projection", projection);
	modelLoading.setMat4("view", view);

	// render sin outlist
	/*
	for (size_t i = 0; i < gobjectsToRender.size(); i++)
	{
		gobjectsToRender[i].Render();
	}
	*/


	for (size_t i = 0; i < gobjectsToRender.size(); i++)
	{
		for (size_t j = 0; j < outList.size(); j++)
		{
			if (gobjectsToRender[i].getID() == outList[j])
			{
				gobjectsToRender[i].Render();
			}
		}
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


	/*
	ImGui::Begin("LOD DEBUG");
	ImGui::Text("GargoylePos: (%.2f, %.2f, %.2f)", gargoyleGO.getPosition().x, gargoyleGO.getPosition().y, gargoyleGO.getPosition().z);
	ImGui::Text("CameraPos: (%.2f, %.2f, %.2f)", camera.Position.x, camera.Position.y, camera.Position.z);
	float distance = glm::distance(gargoyleGO.getPosition(), camera.Position);
	ImGui::Text("Distance: %.2f", distance);
	ImGui::End();
	*/

	/*
	// Instancing gargoyles
	instancing.use();
	instancing.setMat4("projection", projection);
	instancing.setMat4("view", view);

	// para cada mesh del modelo hace un drawInstanced
	for (unsigned int i = 0; i < gargoyle.meshes.size(); i++)
	{
		std::cout << gargoyle.meshes.size() << std::endl;
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		glBindTexture(GL_TEXTURE_2D, 0);
		glDepthFunc(GL_LESS); // depth default

		// Instancing gargoyles
		instancing.use();
		instancing.setMat4("projection", projection);
		instancing.setMat4("view", view);

		for (unsigned int i = 0; i < gargoyle.meshes.size(); i++)
		{
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, gargoyleTexture);
			glBindVertexArray(gargoyle.meshes[i].VAO);
			glDrawElementsInstanced(GL_TRIANGLES, static_cast<unsigned int>(gargoyle.meshes[i].indices.size()), GL_UNSIGNED_INT, 0, amount);
			glBindVertexArray(0);
			glBindTexture(GL_TEXTURE_2D, 0);
		}


		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDisable(GL_DEPTH_TEST);
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		screenShader.use();
		glBindVertexArray(quadVAO);
		glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}
	*/

	// RENDERIZAR A IMGUI
	glBindFramebuffer(GL_FRAMEBUFFER, imguiFBO);
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.f, 0.f, 0.f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// projection / view
	projection = glm::perspective(glm::radians(imguiCamera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, near, far);
	view = glm::lookAt(imguiCamera.Position, imguiCamera.Position + imguiCamera.Front, imguiCamera.Up);

	modelLoading.use();
	modelLoading.setMat4("projection", projection);
	modelLoading.setMat4("view", view);


	for (size_t i = 0; i < outList.size(); i++)
	{
		gobjectsToRender[i].Render();
	}

	// volver a framebuffer principal
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDisable(GL_DEPTH_TEST);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	// dibujar fbo imgui
	ImGui::Begin("TopDown Camera");
	ImGui::Image((ImTextureID)(intptr_t)imguiTextureBuffer, ImVec2(SCR_WIDTH / 3, SCR_HEIGHT / 3), ImVec2(0, 1), ImVec2(1, 0));
	ImGui::DragFloat("X", &posX, 0.5f);
	ImGui::DragFloat("Y", &posY, 0.5f);
	ImGui::DragFloat("Z", &posZ, 0.5f);
	ImGui::End();

	// dibujar un quad para pintarlo en toda la pantalla
	screenShader.use();
	glBindVertexArray(quadVAO);
	glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	glfwSwapBuffers(window);
	glfwPollEvents();
}

ImGui_ImplOpenGL3_Shutdown();
ImGui_ImplGlfw_Shutdown();
ImGui::DestroyContext();

glDeleteVertexArrays(1, &skyboxVAO);
glDeleteBuffers(1, &skyboxVBO);

glfwTerminate();
}

void Renderer::framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
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

	camera.ProcessMouseMovement(xoffset, yoffset);
}

void Renderer::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

void Renderer::processInput(GLFWwindow* window)
{
	if (!moveEnabled) return;

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
}

unsigned int Renderer::loadCubemap(std::vector<std::string> faces)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, nrChannels;
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		}
		else
		{
			std::cout << "Cubemap tex failed to load at path: " << faces[i] << std::endl;
		}
		stbi_image_free(data);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureID;
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

