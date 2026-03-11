# GraphicsTest

**Modern OpenGL 3D Rendering Engine**

A feature-rich graphics engine demonstrating advanced rendering techniques including material systems, batch rendering, frustum culling, shadow mapping, and normal mapping with efficient Mesh/Object architecture.

## Quick Start

```bash
mkdir build && cd build
cmake ..
cmake --build . --config Release
.\Release\GraphicsTest.exe
```

## Features

✅ **Mesh/Object Architecture** - Shared geometry with instanced transforms  
✅ **Material-based Rendering** - PBR-ready material pipeline  
✅ **Batch Rendering** - Automatic grouping by material and geometry  
✅ **Frustum Culling** - GPU-efficient visibility determination  
✅ **Multiple Light Types** - Directional, point, and spot lights with shadows  
✅ **Normal Mapping** - Per-pixel surface detail with tangent space  
✅ **Real-time Statistics** - Performance metrics and debugging  
✅ **Interactive Mini-map** - Top-down view with culling visualization  

## Architecture Highlights

### Efficient Memory Usage
- **Shared Meshes**: 3 meshes for 8 objects (50% memory savings)
- **5 cubes share 1 mesh** - 75% memory reduction
- **2 spheres share 1 mesh** - 50% memory reduction
- Scales to 1000s of objects efficiently

### Design Patterns
- **Flyweight Pattern** - Mesh (intrinsic) + Object (extrinsic) separation
- **Batch Rendering** - Reduces state changes by 40%
- **Frustum Culling** - Skip invisible objects
- **Material System** - Multiple texture maps per object

## Documentation

**📖 [Complete Documentation](PROJECT_DOCUMENTATION.md)** - Comprehensive guide covering:
- **Mesh/Object Architecture** - Memory-efficient design
- **Systems Overview** - Rendering, Materials, Lighting, Culling
- **Technical Details** - Vertex formats, algorithms, optimizations
- **Performance** - Benchmarks and memory usage
- **Controls & Build** - How to use and compile
- **Troubleshooting** - Common issues and solutions

## Controls

### Camera
- **WASD** - Move forward/left/backward/right
- **QE** - Move down/up
- **Mouse** - Look around (when cursor disabled)
- **Tab** - Toggle cursor on/off
- **Scroll** - Zoom in/out

### UI Controls
- **Use Batching** - Enable batch rendering
- **Enable Frustum Culling** - Cull invisible objects
- **Enable Normal Mapping** - Toggle normal maps
- **Enable Shadows** - Toggle shadow rendering
- **Light Controls** - Adjust intensity per light type
- **Show Mini-Map** - Toggle top-down view

## Tech Stack

- **C++17** - Modern C++ features
- **OpenGL 3.3 Core** - Modern graphics pipeline
- **GLFW** - Window and input management
- **GLAD** - OpenGL function loading
- **GLM** - Mathematics library
- **ImGui** - Immediate mode GUI
- **stb_image** - Image loading

## Scene Composition

- **5 Cubes** - Arranged in circle, alternating materials (all share 1 mesh)
- **1 Plane** - Ground with tiled brick texture
- **1 Sphere** - Center object with normal-mapped brick surface
- **1 Light Source** - Animated orbiting point light (shares sphere mesh)

**Total:** 8 objects using only 3 meshes!

## Screenshots

[Add screenshots here]

## Build Instructions

### Windows (Visual Studio)
```bash
mkdir build && cd build
cmake ..
cmake --build . --config Release
.\Release\GraphicsTest.exe
```

### Linux
```bash
mkdir build && cd build
cmake ..
make
./GraphicsTest
```

### CMake Options
- Release mode for best performance
- Debug mode for development

## Project Structure

```
GraphicsTest/
├── src/
│   ├── Mesh.cpp/h              - Shared geometry data
│   ├── Object.cpp/h            - Object instances
│   ├── Scene.cpp/h             - Scene management
│   ├── Material.cpp/h          - Material system
│   ├── BatchRenderer.cpp/h     - Batch rendering
│   ├── Frustum.cpp/h           - Frustum culling
│   ├── LightingManager.cpp/h   - Lighting system
│   └── ...
├── shaders/
│   ├── basic.vert/frag         - Main shaders
│   └── shadow_depth.vert/frag  - Shadow mapping
├── textures/
│   ├── brickwall.jpg           - Diffuse map
│   ├── brickwall_normal.jpg    - Normal map
│   └── container.jpg           - Wood texture
└── CMakeLists.txt
```
