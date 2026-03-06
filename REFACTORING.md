# Graphics Test - Codebase Refactoring

## Overview
This project has been refactored from a monolithic `main.cpp` file into a well-organized, modular architecture following best practices for graphics applications.

## Architecture

### Core Components

#### 1. **Application** (`Application.h/cpp`)
- **Purpose**: Main application controller and entry point
- **Responsibilities**:
  - Window initialization and management
  - Core system initialization (Camera, Scene, Lighting, Input, Renderer)
  - Game loop coordination (Update/Render cycle)
  - Timing and delta time management
- **Key Methods**:
  - `Initialize()`: Sets up all systems
  - `Run()`: Main game loop
  - `Update()`: Updates all game systems
  - `Render()`: Coordinates rendering pipeline

#### 2. **Renderer** (`Renderer.h/cpp`)
- **Purpose**: Handles all rendering operations
- **Responsibilities**:
  - Frame setup and clearing
  - Main viewport rendering
  - Mini-map rendering
  - ImGui UI rendering
  - Viewport management
- **Key Methods**:
  - `BeginFrame()`: Clears buffers
  - `RenderMainView()`: Renders primary scene
  - `RenderMiniMap()`: Renders mini-map viewport
  - `RenderUI()`: Handles ImGui controls
  - `EndFrame()`: Swaps buffers

#### 3. **Scene** (`Scene.h/cpp`)
- **Purpose**: Manages all scene objects
- **Responsibilities**:
  - Creating and storing scene geometry (cubes, plane, sphere)
  - Managing light source visualization
  - Providing unified rendering interface
- **Key Methods**:
  - `Render()`: Draws all scene objects
  - `RenderLightSource()`: Draws the light visualization sphere

#### 4. **LightingManager** (`LightingManager.h/cpp`)
- **Purpose**: Manages all lighting and shadow systems
- **Responsibilities**:
  - Controlling three light types (Directional, Point, Spot)
  - Shadow map generation for all lights
  - Light animation (orbiting point light)
  - Syncing light properties with shaders
- **Key Methods**:
  - `Update()`: Animates lights
  - `RenderShadowMaps()`: Generates depth maps
  - `SetupLighting()`: Configures shader uniforms

#### 5. **InputManager** (`InputManager.h/cpp`)
- **Purpose**: Handles all user input
- **Responsibilities**:
  - Keyboard input processing
  - Mouse movement handling
  - Cursor mode toggling
  - Camera control delegation
- **Key Features**:
  - TAB to toggle cursor mode
  - WASD + Q/E for camera movement
  - ESC to close application

#### 6. **ShadowMap** (`ShadowMap.h/cpp`)
- **Purpose**: Encapsulates shadow mapping functionality
- **Responsibilities**:
  - Framebuffer and depth texture creation
  - Shadow map binding/unbinding
  - Texture unit management
- **Benefits**:
  - Eliminates repetitive shadow map setup code
  - RAII-based resource management (automatic cleanup)

#### 7. **Light** (`Light.h`)
- **Purpose**: Light data structures
- **Contains**:
  - `DirectionalLight`: Sun-like directional lighting
  - `PointLight`: Omnidirectional light with attenuation
  - `SpotLight`: Flashlight-style cone lighting
- **Features**:
  - Automatic light space matrix calculation
  - Unified shader uniform setting

## Benefits of Refactoring

### 1. **Separation of Concerns**
- Each class has a single, well-defined responsibility
- Rendering logic separated from game logic
- Input handling isolated from game state

### 2. **Maintainability**
- Easy to locate and fix bugs
- Clear code organization
- Self-documenting class structure

### 3. **Extensibility**
- Simple to add new features:
  - New light types: Add to `Light.h` and update `LightingManager`
  - New objects: Add to `Scene`
  - New rendering modes: Extend `Renderer`

### 4. **Reusability**
- Classes can be used in other projects
- ShadowMap class eliminates repetitive code
- Light structures are template-ready

### 5. **Testability**
- Each system can be tested independently
- Mock objects can replace dependencies
- Clear interfaces for unit testing

### 6. **Performance**
- No performance degradation
- Better cache locality with organized data
- Easier to profile specific systems

## File Structure

```
GraphicsTest/
├── main.cpp                    # Minimal entry point (17 lines)
├── src/
│   ├── Application.h/cpp       # Main application controller
│   ├── Renderer.h/cpp          # Rendering system
│   ├── Scene.h/cpp             # Scene management
│   ├── LightingManager.h/cpp   # Lighting & shadows
│   ├── InputManager.h/cpp      # Input handling
│   ├── ShadowMap.h/cpp         # Shadow map abstraction
│   ├── Light.h                 # Light data structures
│   ├── Camera.h/cpp            # Camera system (existing)
│   ├── Shader.h/cpp            # Shader system (existing)
│   ├── Cube.h/cpp              # Cube geometry (existing)
│   ├── Plane.h/cpp             # Plane geometry (existing)
│   └── Sphere.h/cpp            # Sphere geometry (existing)
└── shaders/
    ├── basic.vert/frag         # Main shaders
    ├── lighting.frag           # Light source shader
    └── shadow_depth.vert/frag  # Shadow shaders
```

## Code Metrics

### Before Refactoring
- `main.cpp`: ~550 lines
- Functions in global scope: 5
- Global variables: 6
- Shadow map setup: 90 lines (repeated 3x)

### After Refactoring
- `main.cpp`: 17 lines
- Classes: 7 new modular classes
- Average class size: ~100 lines
- Code duplication: Eliminated
- Global variables: 0

## Future Improvements

1. **Configuration System**
   - External config file for settings
   - Runtime adjustable parameters

2. **Resource Manager**
   - Centralized shader/texture management
   - Resource caching and hot-reloading

3. **Entity Component System**
   - More flexible object management
   - Data-oriented design

4. **Post-Processing Pipeline**
   - Bloom, HDR, tone mapping
   - Anti-aliasing (MSAA/FXAA)

5. **Advanced Shadows**
   - Cascaded shadow maps for directional light
   - Omnidirectional shadow maps (cubemaps) for point lights
   - PCF/VSM for soft shadows

## Building

```bash
mkdir build
cd build
cmake ..
cmake --build .
```

## Usage

### Controls
- **WASD**: Move camera horizontally
- **Q/E**: Move camera up/down
- **Mouse**: Look around (when cursor is disabled)
- **TAB**: Toggle cursor mode
- **ESC**: Exit application

### UI Controls
- **Enable Shadows**: Toggle shadow rendering
- **Directional Light**: Toggle sun-like directional light
- **Point Light**: Toggle orbiting point light
- **Spot Light**: Toggle camera flashlight
- **Show Mini-Map**: Toggle mini-map viewport

## Dependencies
- OpenGL 3.3+
- GLFW 3.x
- GLAD
- GLM
- ImGui

## License
[Your License Here]
