# Graphics Test - Complete Project Documentation

**Modern OpenGL Rendering Engine with Advanced Features**

---

## Project Overview

A feature-rich 3D graphics engine built with modern OpenGL (3.3 Core), demonstrating advanced rendering techniques including:
- Material-based rendering system
- Batch rendering with frustum culling
- Multiple light types with shadow mapping
- Normal mapping for surface detail
- Real-time performance visualization

**Tech Stack:**
- C++17
- OpenGL 3.3 Core
- GLFW (windowing)
- GLAD (OpenGL loader)
- GLM (mathematics)
- ImGui (UI)
- stb_image (texture loading)

---

## Features

### Rendering Systems
- ✅ **Material System** - PBR-ready material pipeline with multiple texture maps
- ✅ **Batch Rendering** - Automatic grouping by material and geometry
- ✅ **Frustum Culling** - GPU-efficient visibility determination
- ✅ **Shadow Mapping** - Real-time shadows from all light sources
- ✅ **Normal Mapping** - Per-pixel surface detail
- ✅ **Multiple Lights** - Directional, point, and spot lights

### Visualization
- ✅ **Mini-map** - Top-down view with culling visualization
- ✅ **Statistics** - Real-time rendering metrics
- ✅ **Debug Modes** - Toggle features for comparison
- ✅ **Light Controls** - Adjust intensity per light type

### Objects
- ✅ **Cubes** - 5 instances with alternating materials
- ✅ **Plane** - Ground with tiled brick texture
- ✅ **Sphere** - Center object with normal-mapped surface
- ✅ **Light Source** - Animated orbiting point light

---

## Architecture

### Project Structure
```
GraphicsTest/
├── src/
│   ├── Mesh.cpp/h             - Shared geometry storage
│   ├── Object.cpp/h           - Object instances
│   ├── Application.cpp/h      - Main application loop
│   ├── Renderer.cpp/h         - Rendering orchestration
│   ├── Scene.cpp/h            - Scene management
│   ├── Camera.cpp/h           - FPS camera
│   ├── InputManager.cpp/h     - Input handling
│   ├── LightingManager.cpp/h  - Light and shadow system
│   ├── Material.cpp/h         - Material system
│   ├── Texture.cpp/h          - Texture loading
│   ├── Shader.cpp/h           - Shader compilation
│   ├── BatchRenderer.cpp/h    - Batch rendering
│   ├── Frustum.cpp/h          - Frustum culling
│   ├── ShadowMap.cpp/h        - Shadow map FBO
│   ├── Light.h                - Light structures
│   ├── Cube.cpp/h             - Legacy (can remove)
│   ├── Plane.cpp/h            - Legacy (can remove)
│   └── Sphere.cpp/h           - Legacy (can remove)
├── shaders/
│   ├── basic.vert             - Main vertex shader
│   ├── basic.frag             - Main fragment shader
│   ├── lighting.frag          - Light source shader
│   ├── shadow_depth.vert      - Shadow depth vertex
│   └── shadow_depth.frag      - Shadow depth fragment
├── textures/
│   ├── brickwall.jpg          - Brick diffuse
│   ├── brickwall_normal.jpg   - Brick normal map
│   └── container.jpg          - Wood crate diffuse
└── main.cpp                   - Entry point
```

### 4. Lighting System

**Light Types:**

**Directional Light:**
```cpp
struct DirLight {
    vec3 direction;
    vec3 ambient, diffuse, specular;
    float ambientIntensity, diffuseIntensity, specularIntensity;
};
```
- Simulates sun/moon
- Infinite distance
- Parallel rays
- Casts shadows

**Point Light:**
```cpp
struct PointLight {
    vec3 position;
    float constant, linear, quadratic; // Attenuation
    vec3 ambient, diffuse, specular;
    float ambientIntensity, diffuseIntensity, specularIntensity;
};
```
- Emits in all directions
- Distance-based attenuation
- Animated orbit in scene
- Casts shadows

**Spot Light:**
```cpp
struct SpotLight {
    vec3 position, direction;
    float cutOff, outerCutOff; // Cone angles
    float constant, linear, quadratic;
    vec3 ambient, diffuse, specular;
    float ambientIntensity, diffuseIntensity, specularIntensity;
};
```
- Follows camera (flashlight)
- Cone-shaped emission
- Smooth edge falloff
- Casts shadows

**Lighting Model:**
- **Blinn-Phong** shading
- **Per-fragment** lighting
- **Multiple lights** accumulated
- **Intensity controls** per component

### Shadow Mapping System

**Technique:** Depth map from light's perspective.

**Process:**
1. **Render Pass 1** (Shadow Map Generation):
   ```
   For each light:
     - Bind shadow FBO
     - Set light space matrix
     - Render scene depth only
     - Store in shadow map texture
   ```

2. **Render Pass 2** (Main Rendering):
   ```
   - Bind shadow maps as textures
   - For each fragment:
     - Transform to light space
     - Compare depth with shadow map
     - Apply shadow factor to lighting
   ```

**Features:**
- **PCF (Percentage Closer Filtering)** - Soft shadow edges
- **Bias** - Prevents shadow acne
- **2048x2048** shadow maps per light
- **3 shadow maps** (directional, point, spot)


```

### Normal Mapping System

**Purpose:** Add surface detail without additional geometry.

**How It Works:**

**Tangent Space:**
```
At each vertex, we define a coordinate system:
- Normal (N): Surface normal
- Tangent (T): Direction of U coordinate
- Bitangent (B): Direction of V coordinate

TBN Matrix = [T | B | N]
```

**Normal Map Transformation:**
```glsl
// Sample normal from texture (in tangent space)
vec3 normalMap = texture(material.normalMap, TexCoords).rgb;
normalMap = normalize(normalMap * 2.0 - 1.0); // [0,1] → [-1,1]

// Transform to world space
mat3 TBN = mat3(Tangent, Bitangent, Normal);
vec3 worldNormal = normalize(TBN * normalMap);

// Use worldNormal for lighting calculations
```

**Vertex Format:**
```
Position (3 floats)  : x, y, z
Normal (3 floats)    : nx, ny, nz
TexCoord (2 floats)  : u, v
Tangent (3 floats)   : tx, ty, tz
Bitangent (3 floats) : bx, by, bz
─────────────────────────────────
Total: 14 floats per vertex
```

**Sphere Tangent Calculation:**
```cpp
// Tangent points in direction of increasing U
tangent.x = -sin(u * 2π) * sin(v * π)
tangent.y = 0
tangent.z = cos(u * 2π) * sin(v * π)
tangent = normalize(tangent)

// Bitangent perpendicular to normal and tangent
bitangent = normalize(cross(normal, tangent))
```

**Toggle Control:**
- Checkbox: "Enable Normal Mapping"
- Applies to ALL objects
- Real-time on/off


## Controls

### Camera
- **W/A/S/D** - Move forward/left/backward/right
- **Q/E** - Move down/up
- **Mouse** - Look around
- **Tab** - Toggle mouse cursor (enable/disable camera control)
- **Mouse Scroll** - Zoom (adjust FOV)

### UI Controls

**Light Sources:**
- ☑ Enable Directional Light
- ☑ Enable Point Light
- ☑ Enable Spot Light

**Light Intensity (per light):**
- Ambient: 0.0 - 2.0x
- Diffuse: 0.0 - 2.0x
- Specular: 0.0 - 2.0x

**Rendering Options:**
- ☑ Use Batching
  - ☑ Enable Frustum Culling (only if batching enabled)
- ☑ Enable Normal Mapping
- ☑ Enable Shadows

**View Settings:**
- ☑ Show Mini-Map
  - ⦿ Hide Culled Objects (realistic)
  - ⦿ Show as Red/Green (debug)

**Statistics Display:**
- FPS
- Total Objects
- Visible Objects
- Culled Objects
- Batches (if batching enabled)
- Actual Draw Calls

---

## Build Instructions

### Prerequisites
- CMake 3.10+
- C++17 compatible compiler
- OpenGL 3.3+ support

### Windows (Visual Studio)
```bash
mkdir build
cd build
cmake ..
cmake --build . --config Release
.\Release\GraphicsTest.exe
```

### Linux
```bash
mkdir build
cd build
cmake ..
make
./GraphicsTest
```

### CMake Options
```cmake
# Build in Release for best performance
cmake --build . --config Release

# Build in Debug for development
cmake --build . --config Debug
```

---

## Credits

**Libraries:**
- GLFW - Window and input handling
- GLAD - OpenGL function loading
- GLM - Mathematics library
- ImGui - Immediate mode GUI
- stb_image - Image loading

**Techniques:**
- Learn OpenGL (learnopengl.com)
- OpenGL SuperBible
- Real-Time Rendering

---

## License

This is an educational project. Use freely for learning purposes.

---

