#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <memory>

#include "Shader.h"
#include "Camera.h"
#include "Cube.h"
#include "Plane.h"
#include "Sphere.h"

void checkGLError(const char* location)
{
    GLenum err;
    if ((err = glGetError()) != GL_NO_ERROR)
    {
        std::cout << "OpenGL error at " << location << ": " << err << std::endl;
    }
}

Camera camera(glm::vec3(0.0f, 0.0f, 5.0f));
float lastX = 640.0f;
float lastY = 360.0f;
bool firstMouse = true;
bool cursorDisabled = true;
float deltaTime = 0.0f;
float lastFrame = 0.0f;

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (!cursorDisabled)
        return;

    if (firstMouse)
    {
        lastX = (float)xpos;
        lastY = (float)ypos;
        firstMouse = false;
    }

    float xoffset = (float)xpos - lastX;
    float yoffset = lastY - (float)ypos;
    lastX = (float)xpos;
    lastY = (float)ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    static bool tabPressed = false;
    if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS && !tabPressed)
    {
        tabPressed = true;
        cursorDisabled = !cursorDisabled;
        if (cursorDisabled)
        {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            firstMouse = true;
        }
        else
        {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
    }
    if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_RELEASE)
    {
        tabPressed = false;
    }

    if (!cursorDisabled)
        return;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(0, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(1, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(2, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(3, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        camera.ProcessKeyboard(4, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        camera.ProcessKeyboard(5, deltaTime);
}

void renderScene(const std::vector<std::unique_ptr<Cube>>& cubes, const Plane& plane, const Sphere& sphere, const Shader& shader)
{
    for (const auto& cube : cubes)
    {
        cube->Draw(shader);
    }
    plane.Draw(shader);
    sphere.Draw(shader);
}

int main()
{
    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(1280, 720, "Graphics Test", nullptr, nullptr);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD\n";
        return -1;
    }


	glEnable(GL_DEPTH_TEST);

	Shader shader("../shaders/basic.vert", "../shaders/basic.frag");
	std::cout << "Shader loaded successfully\n";
	std::cout << "Shader ID: " << shader.ID << "\n";

	Shader lightingShader("../shaders/basic.vert", "../shaders/lighting.frag");
	std::cout << "Lighting shader loaded successfully\n";
	std::cout << "Lighting shader ID: " << lightingShader.ID << "\n";

	Shader shadowDepthShader("../shaders/shadow_depth.vert", "../shaders/shadow_depth.frag");
	std::cout << "Shadow depth shader loaded successfully\n";
	std::cout << "Shadow depth shader ID: " << shadowDepthShader.ID << "\n";

	// Create multiple cube instances
	std::vector<std::unique_ptr<Cube>> cubes;
	glm::vec3 coral(0.0f, 0.5f, 0.31f);
	for (int i = 0; i < 5; ++i)
	{
		float angle = (float)i / 5.0f * 360.0f;
		float radius = 3.0f;
		float x = cos(glm::radians(angle)) * radius;
		float z = sin(glm::radians(angle)) * radius;
		//Cube cube = Cube(glm::vec3(x, 0.0f, z));
		cubes.push_back(std::make_unique<Cube>(glm::vec3(x, 0.0f, z), glm::vec3(1.0f), glm::vec3(0.0f), coral));
	}


	std::cout << "Created " << cubes.size() << " cubes\n";

	// Create a ground plane
	Plane plane = Plane(glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(10.0f, 1.0f, 10.0f), glm::vec3(0.0f), glm::vec3(0.3f, 0.3f, 0.3f));
	std::cout << "Created plane\n";

	// Create a sphere
	Sphere sphere = Sphere(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(2.0f), glm::vec3(0.0f), glm::vec3(1.0f, 0.2f, 0.3f));
	std::cout << "Created sphere\n";

	// Create a light source
	Sphere light = Sphere(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.1f), glm::vec3(0.0f), glm::vec3(1.0f, 1.0f, 1.0f));

	// Shadow mapping setup
	const unsigned int SHADOW_WIDTH = 2048, SHADOW_HEIGHT = 2048;

	// Create framebuffers and shadow maps for each light
	unsigned int dirLightDepthMapFBO, dirLightDepthMap;
	glGenFramebuffers(1, &dirLightDepthMapFBO);
	glGenTextures(1, &dirLightDepthMap);
	glBindTexture(GL_TEXTURE_2D, dirLightDepthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	glBindFramebuffer(GL_FRAMEBUFFER, dirLightDepthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, dirLightDepthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	unsigned int pointLightDepthMapFBO, pointLightDepthMap;
	glGenFramebuffers(1, &pointLightDepthMapFBO);
	glGenTextures(1, &pointLightDepthMap);
	glBindTexture(GL_TEXTURE_2D, pointLightDepthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	glBindFramebuffer(GL_FRAMEBUFFER, pointLightDepthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, pointLightDepthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	unsigned int spotLightDepthMapFBO, spotLightDepthMap;
	glGenFramebuffers(1, &spotLightDepthMapFBO);
	glGenTextures(1, &spotLightDepthMap);
	glBindTexture(GL_TEXTURE_2D, spotLightDepthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	glBindFramebuffer(GL_FRAMEBUFFER, spotLightDepthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, spotLightDepthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	std::cout << "Shadow maps created successfully\n";



    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init();

    // ImGui state variables
    bool enableShadows = true;
    bool enableDirLight = true;
    bool enablePointLight = true;
    bool enableSpotLight = true;
    bool showMiniMap = true;

    // Store directional light direction
    glm::vec3 dirLightDirection = glm::vec3(-0.2f, -1.0f, -0.3f);

    // Mini-map camera settings
    glm::vec3 miniMapCameraPos = glm::vec3(0.0f, 15.0f, 15.0f);
    glm::vec3 miniMapCameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
    Camera miniMapCamera(miniMapCameraPos);

    float theta = 0.0f;
    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = (float)glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);
        glfwPollEvents();

        // Update light position
        theta += deltaTime;
        light.Position.x = cos(theta) * 5.0f;
        light.Position.z = sin(theta) * 5.0f;
        light.Position.y = 2.0f;

        // === SHADOW MAP RENDERING ===
        glCullFace(GL_FRONT);

        // 1. Render depth from directional light's perspective
        glm::mat4 dirLightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 1.0f, 20.0f);
        glm::mat4 dirLightView = glm::lookAt(
            -dirLightDirection * 10.0f,
            glm::vec3(0.0f),
            glm::vec3(0.0f, 1.0f, 0.0f)
        );
        glm::mat4 dirLightSpaceMatrix = dirLightProjection * dirLightView;

        shadowDepthShader.use();
        shadowDepthShader.setMat4("lightSpaceMatrix", dirLightSpaceMatrix);

        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, dirLightDepthMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);
        renderScene(cubes, plane, sphere, shadowDepthShader);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // 2. Render depth from point light's perspective
        glm::mat4 pointLightProjection = glm::perspective(glm::radians(90.0f), 1.0f, 1.0f, 25.0f);
        glm::mat4 pointLightView = glm::lookAt(
            light.Position,
            glm::vec3(0.0f),
            glm::vec3(0.0f, 1.0f, 0.0f)
        );
        glm::mat4 pointLightSpaceMatrix = pointLightProjection * pointLightView;

        shadowDepthShader.use();
        shadowDepthShader.setMat4("lightSpaceMatrix", pointLightSpaceMatrix);

        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, pointLightDepthMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);
        renderScene(cubes, plane, sphere, shadowDepthShader);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // 3. Render depth from spot light's perspective
        glm::mat4 spotLightProjection = glm::perspective(glm::radians(45.0f), 1.0f, 1.0f, 25.0f);
        glm::mat4 spotLightView = glm::lookAt(
            camera.Position,
            camera.Position + camera.Front,
            camera.Up
        );
        glm::mat4 spotLightSpaceMatrix = spotLightProjection * spotLightView;

        shadowDepthShader.use();
        shadowDepthShader.setMat4("lightSpaceMatrix", spotLightSpaceMatrix);

        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, spotLightDepthMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);
        renderScene(cubes, plane, sphere, shadowDepthShader);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glCullFace(GL_BACK);

        // === NORMAL RENDERING ===
        glViewport(0, 0, 1280, 720);
        glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        checkGLError("After clear");

        // Setup projection and view matrices
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), 1280.0f / 720.0f, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();

        // Draw Source light
        lightingShader.use();
        lightingShader.setMat4("projection", projection);
        lightingShader.setMat4("view", view);
        light.Draw(lightingShader);


		// Draw cubes BEFORE ImGui
		shader.use();
		checkGLError("After shader.use()");

		shader.setMat4("projection", projection);
		shader.setMat4("view", view);
		shader.setVec3("viewPos", camera.Position);

		// Set shadow and light enable flags
		shader.setBool("enableShadows", enableShadows);
		shader.setBool("enableDirLight", enableDirLight);
		shader.setBool("enablePointLight", enablePointLight);
		shader.setBool("enableSpotLight", enableSpotLight);

		// Bind shadow maps
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, dirLightDepthMap);
		shader.setInt("dirLightShadowMap", 0);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, pointLightDepthMap);
		shader.setInt("pointLightShadowMap", 1);

		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, spotLightDepthMap);
		shader.setInt("spotLightShadowMap", 2);

		// Set light space matrices
		shader.setMat4("dirLightSpaceMatrix", dirLightSpaceMatrix);
		shader.setMat4("pointLightSpaceMatrix", pointLightSpaceMatrix);
		shader.setMat4("spotLightSpaceMatrix", spotLightSpaceMatrix);

		// Directional light (sun-like light from above)
		glm::vec3 dirLightDir = glm::vec3(-0.2f, -1.0f, -0.3f);
		shader.setVec3("dirLight.direction", dirLightDir);
		shader.setVec3("dirLight.ambient", glm::vec3(0.05f, 0.05f, 0.05f));
		shader.setVec3("dirLight.diffuse", glm::vec3(0.4f, 0.4f, 0.4f));
		shader.setVec3("dirLight.specular", glm::vec3(0.5f, 0.5f, 0.5f));

		// Point light (the orbiting sphere's light)
		shader.setVec3("pointLight.position", light.Position);
		shader.setVec3("pointLight.ambient", glm::vec3(0.05f, 0.05f, 0.05f));
		shader.setVec3("pointLight.diffuse", glm::vec3(0.8f, 0.8f, 0.8f));
		shader.setVec3("pointLight.specular", glm::vec3(1.0f, 1.0f, 1.0f));
		shader.setFloat("pointLight.constant", 1.0f);
		shader.setFloat("pointLight.linear", 0.09f);
		shader.setFloat("pointLight.quadratic", 0.032f);

		// Spot light (flashlight from camera)
		shader.setVec3("spotLight.position", camera.Position);
		shader.setVec3("spotLight.direction", camera.Front);
		shader.setVec3("spotLight.ambient", glm::vec3(0.0f, 0.0f, 0.0f));
		shader.setVec3("spotLight.diffuse", glm::vec3(1.0f, 1.0f, 1.0f));
		shader.setVec3("spotLight.specular", glm::vec3(1.0f, 1.0f, 1.0f));
		shader.setFloat("spotLight.constant", 1.0f);
		shader.setFloat("spotLight.linear", 0.09f);
		shader.setFloat("spotLight.quadratic", 0.032f);
		shader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
		shader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));

		// Draw all objects
		for (auto& cube : cubes)
		{
			cube->Draw(shader);
			checkGLError("After cube.Draw()");
		}

		plane.Draw(shader);
		checkGLError("After plane.Draw()");

		sphere.Draw(shader);
		checkGLError("After sphere.Draw()");

		// === MINI-MAP RENDERING ===
		if (showMiniMap)
		{
			// Define mini-map viewport (bottom-right corner)
			int miniMapWidth = 320;
			int miniMapHeight = 240;
			int miniMapX = 1280 - miniMapWidth - 10;
			int miniMapY = 10;

			// Enable scissor test to clear only mini-map area
			glEnable(GL_SCISSOR_TEST);
			glScissor(miniMapX, miniMapY, miniMapWidth, miniMapHeight);
			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glDisable(GL_SCISSOR_TEST);

			// Set viewport for mini-map
			glViewport(miniMapX, miniMapY, miniMapWidth, miniMapHeight);

			// Mini-map camera looking at scene from above/angle
			glm::mat4 miniMapView = glm::lookAt(
				miniMapCameraPos,
				miniMapCameraTarget,
				glm::vec3(0.0f, 1.0f, 0.0f)
			);
			glm::mat4 miniMapProjection = glm::perspective(
				glm::radians(45.0f),
				(float)miniMapWidth / (float)miniMapHeight,
				0.1f,
				100.0f
			);

			// Draw light source in mini-map
			lightingShader.use();
			lightingShader.setMat4("projection", miniMapProjection);
			lightingShader.setMat4("view", miniMapView);
			light.Draw(lightingShader);

			// Draw scene in mini-map
			shader.use();
			shader.setMat4("projection", miniMapProjection);
			shader.setMat4("view", miniMapView);

			for (auto& cube : cubes)
			{
				cube->Draw(shader);
			}
			plane.Draw(shader);
			sphere.Draw(shader);

			// Draw a border around mini-map using line loop
			glViewport(0, 0, 1280, 720);
			glDisable(GL_DEPTH_TEST);

			// Simple border (you could make this fancier with a quad shader)
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			glEnable(GL_DEPTH_TEST);
		}

		// Reset viewport for ImGui
		glViewport(0, 0, 1280, 720);



        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        checkGLError("After ImGui::NewFrame()");

        // Create lighting controls window
        ImGui::Begin("Lighting Controls");
        ImGui::Text("Press TAB to toggle cursor");
        ImGui::Separator();
        ImGui::Text("Shadow Settings");
        ImGui::Checkbox("Enable Shadows", &enableShadows);
        ImGui::Separator();
        ImGui::Text("Light Sources");
        ImGui::Checkbox("Directional Light", &enableDirLight);
        ImGui::Checkbox("Point Light", &enablePointLight);
        ImGui::Checkbox("Spot Light", &enableSpotLight);
        ImGui::Separator();
        ImGui::Text("View Settings");
        ImGui::Checkbox("Show Mini-Map", &showMiniMap);
        ImGui::Separator();
        ImGui::Text("Application %.1f FPS", ImGui::GetIO().Framerate);
        ImGui::End();

        ImGui::ShowDemoWindow();
        checkGLError("After ImGui::ShowDemoWindow()");

        ImGui::Render();
        checkGLError("After ImGui::Render()");
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        checkGLError("After ImGui_ImplOpenGL3_RenderDrawData()");

        glfwSwapBuffers(window);
        checkGLError("After glfwSwapBuffers()");
    }
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();
    return 0;
}
