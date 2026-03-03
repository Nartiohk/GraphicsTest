#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

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
float deltaTime = 0.0f;
float lastFrame = 0.0f;

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
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

	glViewport(0, 0, 1280, 720);
	glEnable(GL_DEPTH_TEST);

	Shader shader("../shaders/basic.vert", "../shaders/basic.frag");
	std::cout << "Shader loaded successfully\n";
	std::cout << "Shader ID: " << shader.ID << "\n";

    Shader lightingShader("../shaders/basic.vert", "../shaders/lighting.frag");
    std::cout << "Shader loaded successfully\n";
    std::cout << "Shader ID: " << lightingShader.ID << "\n";

	// Create multiple cube instances
	std::vector<std::unique_ptr<Cube>> cubes;
	glm::vec3 coral(0.0f, 0.5f, 0.31f);
	Cube cube = Cube(glm::vec3(0.0f, 0.0f, 0.0));
	// Center cube
	//Cube cube = Cube(glm::vec3(0.0f, 0.0f, 0.0f));
	//cube.Color = glm::vec3(1.0f, 0.5f, 0.2f);
	//cubes.push_back(std::make_unique<Cube>(cube));
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

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);          // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
    ImGui_ImplOpenGL3_Init();

    float theta = 0.0f;
    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = (float)glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);
        glfwPollEvents();

        // Rendering - Clear buffers first
        glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        checkGLError("After clear");

        // Setup projection and view matrices
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), 1280.0f / 720.0f, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();


        //Draw Source light
        lightingShader.use();
        lightingShader.setMat4("projection", projection);
        lightingShader.setMat4("view", view);
        //lightingShader.setVec3("lightColor", glm::vec3(1.0f));
        theta += deltaTime; // Rotate at 50 degrees per second
        cube.Position.x = cos(theta) * 5.0f; // No rotation for the light source cube
        cube.Position.z = sin(theta) * 5.0f; // No rotation for the light source cube
        //lightingShader.setVec3("lightPos", cube.Position);
        cube.Draw(lightingShader);



		// Draw cubes BEFORE ImGui
		shader.use();
		checkGLError("After shader.use()");

		shader.setMat4("projection", projection);
		checkGLError("After setMat4 projection");
		shader.setMat4("view", view);
		shader.setVec3("viewPos", camera.Position);
		checkGLError("After setMat4 view");

		// Directional light (sun-like light from above)
		shader.setVec3("dirLight.direction", glm::vec3(-0.2f, -1.0f, -0.3f));
		shader.setVec3("dirLight.ambient", glm::vec3(0.05f, 0.05f, 0.05f));
		shader.setVec3("dirLight.diffuse", glm::vec3(0.4f, 0.4f, 0.4f));
		shader.setVec3("dirLight.specular", glm::vec3(0.5f, 0.5f, 0.5f));

		// Point light (the orbiting cube's light)
		shader.setVec3("pointLight.position", cube.Position);
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


        // Draw all cubes
		//shader.setVec3("lightPos", cube.Position);
        for (auto& cube : cubes)
        {
            //cube->Rotation.y += 20.0f * deltaTime;
            //cube->Rotation.x += 10.0f * deltaTime;
            cube->Draw(shader);
            checkGLError("After cube.Draw()");
        }

        // Draw the plane
        plane.Draw(shader);
        checkGLError("After plane.Draw()");

        // Draw the sphere
        sphere.Draw(shader);
        checkGLError("After sphere.Draw()");


        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        checkGLError("After ImGui::NewFrame()");

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