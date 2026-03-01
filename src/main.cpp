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

	// Create multiple cube instances
    std::vector<std::unique_ptr<Cube>> cubes;
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
        cubes.push_back(std::make_unique<Cube>(glm::vec3(x, 0.0f, z)));
    }

	//// Cube to the right
	//cubes.push_back(Cube(glm::vec3(2.5f, 0.0f, 0.0f)));
	//cubes.back().Color = glm::vec3(0.2f, 0.5f, 1.0f);

	//// Cube to the left
	//cubes.push_back(Cube(glm::vec3(-2.5f, 0.0f, 0.0f)));
	//cubes.back().Color = glm::vec3(0.2f, 1.0f, 0.5f);

	//// Cube above
	//cubes.push_back(Cube(glm::vec3(0.0f, 2.5f, 0.0f)));
	//cubes.back().Color = glm::vec3(1.0f, 0.2f, 0.5f);

	//// Cube below
	//cubes.push_back(Cube(glm::vec3(0.0f, -2.5f, 0.0f)));
	//cubes.back().Color = glm::vec3(1.0f, 1.0f, 0.2f);

	//// Cube in front
	//cubes.push_back(Cube(glm::vec3(0.0f, 0.0f, 2.5f), glm::vec3(0.5f)));
	//cubes.back().Color = glm::vec3(0.8f, 0.2f, 1.0f);

	//// Cube behind
	//cubes.push_back(Cube(glm::vec3(0.0f, 0.0f, -2.5f), glm::vec3(1.5f)));
	//cubes.back().Color = glm::vec3(0.5f, 1.0f, 1.0f);

	std::cout << "Created " << cubes.size() << " cubes\n";

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);          // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
    ImGui_ImplOpenGL3_Init();
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

        // Draw cubes BEFORE ImGui
        shader.use();
        checkGLError("After shader.use()");

        // Setup projection and view matrices
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), 1280.0f / 720.0f, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();

        shader.setMat4("projection", projection);
        checkGLError("After setMat4 projection");
        shader.setMat4("view", view);
        checkGLError("After setMat4 view");

        // Draw all cubes
        for (auto& cube : cubes)
        {
            cube->Rotation.y += 20.0f * deltaTime;
            cube->Rotation.x += 10.0f * deltaTime;
            cube->Draw(shader);
            checkGLError("After cube.Draw()");
        }

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