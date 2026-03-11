#include "Application.h"
#include <glad/glad.h>
#include <iostream>

Application::Application(int width, int height)
    : m_Window(nullptr)
    , m_Width(width)
    , m_Height(height)
    , m_DeltaTime(0.0f)
    , m_LastFrame(0.0f)
    , m_ShowMiniMap(true)
    , m_UseBatching(true)
    , m_EnableCulling(true)
    , m_HideCulledInMinimap(true)
    , m_EnableNormalMapping(true)
{
}

Application::~Application()
{
    if (m_Window)
    {
        glfwTerminate();
    }
}

bool Application::Initialize()
{
    // Initialize GLFW
    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return false;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create window
    m_Window = glfwCreateWindow(m_Width, m_Height, "Graphics Test", nullptr, nullptr);
    if (!m_Window)
    {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(m_Window);
    glfwSetFramebufferSizeCallback(m_Window, FramebufferSizeCallback);

    // Initialize GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return false;
    }

    // Enable depth testing
    glEnable(GL_DEPTH_TEST);

    // Initialize shaders
    m_MainShader = std::make_unique<Shader>("../shaders/basic.vert", "../shaders/basic.frag");
    std::cout << "Main shader loaded (ID: " << m_MainShader->ID << ")" << std::endl;

    m_LightingShader = std::make_unique<Shader>("../shaders/basic.vert", "../shaders/lighting.frag");
    std::cout << "Lighting shader loaded (ID: " << m_LightingShader->ID << ")" << std::endl;

    m_ShadowDepthShader = std::make_unique<Shader>("../shaders/shadow_depth.vert", "../shaders/shadow_depth.frag");
    std::cout << "Shadow depth shader loaded (ID: " << m_ShadowDepthShader->ID << ")" << std::endl;

    m_CubeShadowDepthShader = std::make_unique<Shader>("../shaders/cube_shadow_depth.vert", "../shaders/cube_shadow_depth.frag");
    std::cout << "Cube shadow depth shader loaded (ID: " << m_CubeShadowDepthShader->ID << ")" << std::endl;

    // Initialize cameras
    m_Camera = std::make_unique<Camera>(glm::vec3(0.0f, 0.0f, 5.0f));
    m_MiniMapCamera = std::make_unique<Camera>(glm::vec3(0.0f, 15.0f, 15.0f));

    // Initialize scene
    m_Scene = std::make_unique<Scene>();
    std::cout << "Scene created" << std::endl;

    // Initialize lighting manager
    m_LightingManager = std::make_unique<LightingManager>();
    m_LightingManager->SetShadowDepthShader(m_ShadowDepthShader.get());
    m_LightingManager->SetCubeShadowDepthShader(m_CubeShadowDepthShader.get());
    std::cout << "Lighting manager created" << std::endl;

    // Initialize input manager
    m_InputManager = std::make_unique<InputManager>(m_Window, *m_Camera);
    InputManager::SetInstance(m_InputManager.get());
    glfwSetCursorPosCallback(m_Window, InputManager::MouseCallback);
    std::cout << "Input manager created" << std::endl;

    // Initialize renderer
    m_Renderer = std::make_unique<Renderer>(m_Window, m_Width, m_Height);
    std::cout << "Renderer created" << std::endl;

    return true;
}

void Application::Run()
{
    while (!glfwWindowShouldClose(m_Window))
    {
        // Calculate delta time
        float currentFrame = (float)glfwGetTime();
        m_DeltaTime = currentFrame - m_LastFrame;
        m_LastFrame = currentFrame;

        // Process input
        m_InputManager->ProcessInput(m_DeltaTime);
        glfwPollEvents();

        // Update
        Update(m_DeltaTime);

        // Render
        Render();
    }
}

void Application::Update(float deltaTime)
{
    // Update lighting (animates point light)
    m_LightingManager->Update(deltaTime);

    // Sync spotlight with camera
    auto& spotLight = m_LightingManager->GetSpotLight();
    spotLight.position = m_Camera->Position;
    spotLight.direction = m_Camera->Front;

    // Sync light source sphere with point light
    auto& pointLight = m_LightingManager->GetPointLight();
    m_Scene->GetLightSource().Position = pointLight.position;
}

void Application::Render()
{
    // Render shadow maps
    auto renderScene = [this](const Shader& shader) {
        m_Scene->Render(shader);
    };
    m_LightingManager->RenderShadowMaps(renderScene);

    // Begin main rendering
    m_Renderer->BeginFrame();

    // Render main view and get frustum
    Frustum mainCameraFrustum = m_Renderer->RenderMainView(*m_Camera, *m_MainShader, *m_LightingShader, 
                                                            *m_Scene, *m_LightingManager, m_UseBatching, 
                                                            m_EnableCulling, m_EnableNormalMapping);

    // Render mini-map with culling visualization
    if (m_ShowMiniMap)
    {
        // Pass enableCulling so minimap respects the culling setting
        m_Renderer->RenderMiniMap(*m_MainShader, *m_LightingShader, *m_Scene, mainCameraFrustum, 
                                  m_UseBatching, m_EnableCulling, m_HideCulledInMinimap);
    }

    // Render UI
    m_Renderer->RenderUI(*m_LightingManager, m_ShowMiniMap, m_UseBatching, m_EnableCulling, 
                        m_HideCulledInMinimap, m_EnableNormalMapping, *m_Scene);

    // End frame
    m_Renderer->EndFrame();
}

void Application::FramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}
