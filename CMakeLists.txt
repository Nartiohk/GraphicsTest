cmake_minimum_required(VERSION 3.15)
project(GraphicsTest)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(thirdparty/glfw)

add_library(glad thirdparty/glad/src/glad.c)
target_include_directories(glad PUBLIC thirdparty/glad/include)

file(GLOB IMGUI_SRC
    thirdparty/imgui/*.cpp
    thirdparty/imgui/backends/imgui_impl_glfw.cpp
    thirdparty/imgui/backends/imgui_impl_opengl3.cpp
)

add_library(imgui ${IMGUI_SRC})
target_include_directories(imgui PUBLIC
    thirdparty/imgui
    thirdparty/imgui/backends
    thirdparty/glfw/include
    thirdparty/glad/include
)

add_executable(GraphicsTest
    main.cpp
    src/Shader.cpp
    src/Shader.h
    src/Camera.cpp
    src/Camera.h
    src/Texture.cpp
    src/Texture.h
    src/Material.cpp
    src/Material.h
    src/ShadowMap.cpp
    src/ShadowMap.h
    src/CubeShadowMap.cpp
    src/CubeShadowMap.h
    src/Light.h
    src/LightingManager.cpp
    src/LightingManager.h
    src/Scene.cpp
    src/Scene.h
    src/InputManager.cpp
    src/InputManager.h
    src/Renderer.cpp
    src/Renderer.h
    src/Application.cpp
    src/Application.h
    src/Frustum.cpp
    src/Frustum.h
    src/BatchRenderer.cpp
    src/BatchRenderer.h
    src/Mesh.cpp
    src/Mesh.h
    src/Object.cpp
    src/Object.h
)

target_include_directories(GraphicsTest PRIVATE
    src
    thirdparty/glad/include
    thirdparty/glm
    thirdparty/glfw/include
    thirdparty/stb
)

target_link_libraries(GraphicsTest
    glfw
    glad
    imgui
)

# Platform-specific OpenGL linking
if(WIN32)
    target_link_libraries(GraphicsTest opengl32)
elseif(UNIX AND NOT APPLE)
    target_link_libraries(GraphicsTest GL dl pthread)
elseif(APPLE)
    target_link_libraries(GraphicsTest "-framework OpenGL")
endif()

# Find Shader Files
file(GLOB_RECURSE SHADER_FILES "shaders/*")

# Add Target GraphicsTest
target_sources(GraphicsTest PRIVATE ${SHADER_FILES})

# Grouping "Shaders" in Visual Studio
source_group("shaders" FILES ${SHADER_FILES})

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT GraphicsTest)

set_target_properties(glad imgui glfw PROPERTIES FOLDER "Libraries")