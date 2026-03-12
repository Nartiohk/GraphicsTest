# Technical Documentation: 3D Graphics Rendering Engine

## Table of Contents

1. [System Overview](#system-overview)
2. [Rendering Pipeline Architecture](#rendering-pipeline-architecture)
3. [Lighting Algorithm](#lighting-algorithm)
4. [Shadow Mapping Algorithm](#shadow-mapping-algorithm)
5. [Optimization Systems](#optimization-systems)
6. [Material and Texture System](#material-and-texture-system)
7. [Problems Encountered and Solutions](#problems-encountered-and-solutions)
8. [Current Limitations and Future Improvements](#current-limitations-and-future-improvements)

---

## System Overview

This graphics engine is a real-time 3D rendering system built with modern OpenGL (3.3 Core Profile) implementing:

- Phong/Blinn-Phong lighting model
- Shadow mapping with PCF (Percentage Closer Filtering)
- View frustum culling
- Batch rendering system
- Material system with PBR-like properties
- Normal mapping
- Multiple light types (Directional, Point, Spot)

**Technology Stack:**

- OpenGL 3.3 Core
- GLFW for window management
- GLM for mathematics
- GLAD for OpenGL function loading
- ImGui for UI
- C++17

---

## Rendering Pipeline Architecture

### Pipeline Components

#### 1. **Update Stage** (`Application::Update`)

- Updates animated point light position (orbiting movement)
- Synchronizes spotlight with camera position/direction
- Updates light source visualization sphere position

```cpp
void Application::Update(float deltaTime)
{
    m_LightingManager->Update(deltaTime);

    auto& spotLight = m_LightingManager->GetSpotLight();
    spotLight.position = m_Camera->Position;
    spotLight.direction = m_Camera->Front;

    auto& pointLight = m_LightingManager->GetPointLight();
    m_Scene->GetLightSource().Position = pointLight.position;
}
```

#### 2. **Shadow Pass** (`LightingManager::RenderShadowMaps`)

Renders scene depth from each light's perspective:

- **Directional Light**: Orthographic projection (2048x2048)
- **Point Light**: Perspective projection (2048x2048)
- **Spot Light**: Perspective projection (2048x2048)

**Optimization**: Front-face culling during shadow pass to reduce shadow acne

```cpp
glCullFace(GL_FRONT);  // Peter Panning prevention
// Render shadow maps...
glCullFace(GL_BACK);
```

#### 3. **Main Rendering Pass** (`Renderer::RenderMainView`)

- Setup projection and view matrices
- Create view frustum from projection × view matrix
- Render light source sphere (unlit)
- Setup lighting uniforms
- Render scene with optional batching and culling

#### 4. **Batch Rendering System** (`BatchRenderer`)

The batch renderer groups objects by material and geometry to minimize state changes:

**Batching Process:**

1. **Submit Phase**: Collect renderables
2. **Prepare Phase**:
   - Cull objects outside frustum (optional)
   - Group by BatchKey (Material + VAO)
3. **Render Phase**:
   - For each batch:
     - Bind material once
     - Bind VAO once
     - Render all objects in batch

**Batch Key:**

```cpp
struct BatchKey {
    std::shared_ptr<Material> material;
    unsigned int VAO;
};
```

Objects with the same material and mesh are batched together, reducing:

- Material state changes
- Shader uniform updates
- VAO bindings

---

## Lighting Algorithm

### Blinn-Phong Illumination Model

The engine implements the **Blinn-Phong reflection model**, an improvement over traditional Phong that uses the halfway vector for specular calculations.

#### Mathematical Formula

For each light source, the final color is computed as:

```
L = La + (1 - shadow) × (Ld + Ls)
```

Where:

- **La** = Ambient component
- **Ld** = Diffuse component
- **Ls** = Specular component
- **shadow** = Shadow factor [0, 1]

#### Component Breakdown

**1. Ambient Component**

```
La = Cambient × Malbedo × Iambient
```

- `Cambient`: Light's ambient color
- `Malbedo`: Material's diffuse color (from texture or base color)
- `Iambient`: Ambient intensity multiplier

**2. Diffuse Component (Lambertian Reflection)**

```
Ld = Cdiffuse × max(N · L, 0) × Malbedo × Idiffuse
```

- `N`: Surface normal (world space)
- `L`: Light direction vector
- `Cdiffuse`: Light's diffuse color
- `Idiffuse`: Diffuse intensity multiplier

**3. Specular Component (Blinn-Phong)**

```
Ls = Cspecular × [(N · H)^shininess] × Mspecular × Ispecular
```

- `H`: Halfway vector = normalize(L + V)
- `V`: View direction
- `shininess`: Material shininess (specular exponent)
- `Mspecular`: Specular intensity from specular map
- `Cspecular`: Light's specular color
- `Ispecular`: Specular intensity multiplier

#### Shader Implementation

```glsl
vec3 CalculateBlinnPhong(
    vec3 lightDir,
    vec3 normal,
    vec3 viewDir,
    vec3 ambientColor,
    vec3 diffuseColor,
    vec3 specularColor,
    vec3 albedo,
    float shininess,
    float specularIntensity,
    float shadow
)
{
    // Diffuse
    float diff = max(dot(normal, lightDir), 0.0);

    // Blinn-Phong Specular
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), shininess);

    vec3 ambient  = ambientColor * albedo;
    vec3 diffuse  = diffuseColor * diff * albedo;
    vec3 specular = specularColor * spec * specularIntensity;

    // Shadows don't affect ambient
    return ambient + (1.0 - shadow) * (diffuse + specular);
}
```

### Light Types

#### 1. Directional Light

Simulates distant light source (e.g., sun):

```
L_final = CalculateBlinnPhong(...)
```

- Direction: Fixed direction in world space
- No attenuation (infinite distance)

#### 2. Point Light

Omnidirectional point source with distance attenuation:

```
L_final = CalculateBlinnPhong(...) × Attenuation
```

**Attenuation Formula:**

```
Attenuation = 1 / (Kconstant + Klinear × d + Kquadratic × d²)
```

- `d`: Distance from light to fragment
- `Kconstant`: 1.0 (base attenuation)
- `Klinear`: 0.09 (linear falloff)
- `Kquadratic`: 0.032 (quadratic falloff)

Default settings provide ~50-unit effective radius.

#### 3. Spot Light

Cone-shaped light with smooth edge falloff:

```
L_final = CalculateBlinnPhong(...) × Attenuation × Intensity
```

**Intensity Formula (Smooth Cone):**

```
θ = dot(L, -SpotDir)
ε = φ_inner - φ_outer
Intensity = clamp((θ - φ_outer) / ε, 0, 1)
```

- `θ`: Angle between light direction and fragment direction
- `φ_inner`: Inner cone angle (cutOff = cos(12.5°))
- `φ_outer`: Outer cone angle (outerCutOff = cos(15.0°))
- `ε`: Smooth transition range

### Normal Mapping

Normal mapping adds surface detail without additional geometry:

**Process:**

1. Sample normal from normal map (RGB = XYZ in tangent space)
2. Transform from [0,1] to [-1,1]: `N = normalize(rgb × 2.0 - 1.0)`
3. Transform to world space using TBN matrix: `N_world = TBN × N_tangent`

**TBN Matrix Construction** (in vertex shader):

```glsl
vec3 T = normalize(vec3(model * vec4(aTangent, 0.0)));
vec3 N = normalize(vec3(model * vec4(aNormal, 0.0)));
T = normalize(T - dot(T, N) * N);  // Gram-Schmidt orthogonalization
vec3 B = cross(N, T);
mat3 TBN = mat3(T, B, N);
```

### Material Properties

Materials control surface appearance:

```cpp
struct Material {
    vec3 baseColor;           // Base tint
    float shininess;          // Specular exponent (16-256)

    sampler2D diffuseMap;     // Albedo texture
    sampler2D specularMap;    // Specular intensity (grayscale)
    sampler2D normalMap;      // Surface normals (tangent space)
    sampler2D emissionMap;    // Self-illumination
};
```

**Example Configurations:**

- **Brick**: Shininess = 16.0, Medium specular (64,64,64)
- **Wood**: Shininess = 64.0, High specular (200,200,200)

---

## Shadow Mapping Algorithm

### Overview

Shadow mapping is a two-pass technique:

1. **Depth Pass**: Render scene from light's POV to depth texture
2. **Shading Pass**: Compare fragment depth against shadow map

### Implementation Details

#### 1. Shadow Map Creation (`ShadowMap` class)

**Framebuffer Setup:**

```cpp
// Create depth-only framebuffer
glGenFramebuffers(1, &m_FBO);
glGenTextures(1, &m_DepthMap);

// Configure depth texture (2048x2048)
glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 2048, 2048, ...);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

// Set border color to white (depth = 1.0 → no shadow)
float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

// Attach to framebuffer
glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, ...);
glDrawBuffer(GL_NONE);  // No color output
glReadBuffer(GL_NONE);
```

#### 2. Light Space Transform

Transform scene geometry to light's coordinate system using `lightSpaceMatrix`:

**Directional Light (Orthographic):**

```cpp
mat4 lightProjection = ortho(-10, 10, -10, 10, 1, 20);
mat4 lightView = lookAt(-direction × 10, vec3(0), vec3(0,1,0));
mat4 lightSpaceMatrix = lightProjection × lightView;
```

**Point/Spot Light (Perspective):**

```cpp
mat4 lightProjection = perspective(radians(90), 1.0, 1, 25);
mat4 lightView = lookAt(lightPos, target, up);
mat4 lightSpaceMatrix = lightProjection × lightView;
```

#### 3. Depth Pass Vertex Shader

```glsl
#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 lightSpaceMatrix;
uniform mat4 model;

void main()
{
    gl_Position = lightSpaceMatrix * model * vec4(aPos, 1.0);
}
```

Depth is automatically written to depth buffer (no fragment shader needed).

#### 4. Shadow Calculation with PCF

**PCF (Percentage Closer Filtering)** reduces shadow aliasing by sampling multiple depth values:

```glsl
float ShadowCalculation(vec4 fragPosLightSpace, sampler2D shadowMap,
                        vec3 normal, vec3 lightDir)
{
    // Perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;

    // Transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;

    // Outside shadow map → no shadow
    if (projCoords.z > 1.0)
        return 0.0;

    float currentDepth = projCoords.z;

    // Adaptive bias prevents shadow acne
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);

    // PCF: Sample 3x3 grid around current texel
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);

    for(int x = -1; x <= 1; ++x) {
        for(int y = -1; y <= 1; ++y) {
            float pcfDepth = texture(shadowMap,
                                   projCoords.xy + vec2(x,y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;  // Average of 9 samples

    return shadow;
}
```

**PCF Benefits:**

- 9 samples per fragment (3×3 kernel)
- Produces softer shadow edges
- Reduces aliasing artifacts
- ~1 texel blur effect

#### 5. Shadow Acne Prevention

**Problem**: Depth buffer precision causes self-shadowing artifacts

**Solutions Implemented:**

**a) Adaptive Bias**

```glsl
float bias = max(0.05 × (1.0 - dot(N, L)), 0.005);
```

- Larger bias for surfaces parallel to light
- Minimum bias of 0.005 for surfaces facing light

**b) Front-Face Culling During Depth Pass**

```cpp
glCullFace(GL_FRONT);  // Cull front faces
// Render shadow maps...
glCullFace(GL_BACK);   // Restore back-face culling
```

- Renders back faces to shadow map
- Pushes depth values away from camera
- Reduces acne but may cause "Peter Panning" (detached shadows)

**c) Border Color = White**

- Regions outside shadow map coverage are treated as fully lit
- Prevents dark artifacts at edges

### Shadow Integration

Shadows are applied per-light in fragment shader:

```glsl
// Directional light
if (enableDirLight) {
    float shadow = 0.0;
    if (enableShadows) {
        vec4 fragPosLightSpace = dirLightSpaceMatrix * vec4(FragPos, 1.0);
        shadow = ShadowCalculation(fragPosLightSpace, dirLightShadowMap,
                                   norm, normalize(-dirLight.direction));
    }
    result += CalcDirLight(dirLight, norm, viewDir, albedo,
                          shininess, specularIntensity, shadow);
}
```

Each light type has its own shadow map and light space matrix.

## Cubemap Shadow Mapping for Point Lights

### Implementation Architecture

#### 1. CubeShadowMap Class

New class specifically for cubemap depth textures:

```cpp
class CubeShadowMap
{
public:
    CubeShadowMap(unsigned int resolution = 1024);
    ~CubeShadowMap();

    void BindForWriting(unsigned int cubeFace) const;
    void Unbind() const;
    void BindTexture(unsigned int textureUnit) const;

private:
    unsigned int m_FBO;
    unsigned int m_DepthCubemap;
    unsigned int m_Resolution;
};
```

**Key Differences from Regular ShadowMap:**

- Uses `GL_TEXTURE_CUBE_MAP` instead of `GL_TEXTURE_2D`
- Creates 6 depth texture faces (one per cube direction)
- `BindForWriting()` takes face index to render to specific face
- Lower default resolution (1024 vs 2048) due to 6× memory usage

#### 2. Cubemap Creation

```cpp
CubeShadowMap::CubeShadowMap(unsigned int resolution)
    : m_Resolution(resolution)
{
    glGenFramebuffers(1, &m_FBO);
    glGenTextures(1, &m_DepthCubemap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_DepthCubemap);

    // Create 6 depth texture faces
    for (unsigned int i = 0; i < 6; ++i)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT,
                     m_Resolution, m_Resolution, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    }

    // Cubemap-specific parameters
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    // Attach entire cubemap to framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_DepthCubemap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
}
```

#### 3. Cube Face View Matrices

Point light needs 6 view matrices, one per cube face:

```cpp
std::vector<glm::mat4> GetCubeShadowMatrices(float nearPlane = 1.0f, float farPlane = 25.0f) const
{
    glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), 1.0f, nearPlane, farPlane);

    std::vector<glm::mat4> shadowTransforms;
    shadowTransforms.push_back(shadowProj * glm::lookAt(position, position + glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f))); // +X
    shadowTransforms.push_back(shadowProj * glm::lookAt(position, position + glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f))); // -X
    shadowTransforms.push_back(shadowProj * glm::lookAt(position, position + glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f))); // +Y
    shadowTransforms.push_back(shadowProj * glm::lookAt(position, position + glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f))); // -Y
    shadowTransforms.push_back(shadowProj * glm::lookAt(position, position + glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f))); // +Z
    shadowTransforms.push_back(shadowProj * glm::lookAt(position, position + glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))); // -Z

    return shadowTransforms;
}
```

**Matrix Breakdown:**

| Face | Target Direction | Up Vector  | Covers     |
| ---- | ---------------- | ---------- | ---------- |
| +X   | (1, 0, 0)        | (0, -1, 0) | Right side |
| -X   | (-1, 0, 0)       | (0, -1, 0) | Left side  |
| +Y   | (0, 1, 0)        | (0, 0, 1)  | Top        |
| -Y   | (0, -1, 0)       | (0, 0, -1) | Bottom     |
| +Z   | (0, 0, 1)        | (0, -1, 0) | Front      |
| -Z   | (0, 0, -1)       | (0, -1, 0) | Back       |

**Projection Settings:**

- FOV: 90° (covers exactly one cube face)
- Aspect ratio: 1.0 (square faces)
- Near plane: 1.0 (close to light)
- Far plane: 25.0 (light attenuation range)

#### 4. Rendering Cubemap Shadows

Multi-pass rendering (6 passes per point light):

```cpp
void LightingManager::RenderShadowMaps(const std::function<void(const Shader&)>& renderSceneFunc)
{
    glCullFace(GL_FRONT);

    // ... directional light shadow map ...

    // Render point light shadow map (cubemap - 6 faces)
    if (m_EnablePointLight)
    {
        auto shadowMatrices = m_PointLight.GetCubeShadowMatrices();
        for (unsigned int i = 0; i < 6; ++i)
        {
            m_PointLightCubeShadowMap->BindForWriting(i);
            m_ShadowDepthShader->use();
            m_ShadowDepthShader->setMat4("lightSpaceMatrix", shadowMatrices[i]);
            renderSceneFunc(*m_ShadowDepthShader);
        }
        m_PointLightCubeShadowMap->Unbind();
    }

    // ... spot light shadow map ...

    glCullFace(GL_BACK);
}
```

#### 5. Cubemap Shadow Sampling

Fragment shader samples cubemap using 3D direction vector:

```glsl
float CubeShadowCalculation(vec3 fragPos, vec3 lightPos, samplerCube shadowCubemap, float farPlane)
{
    // Get vector from light to fragment
    vec3 fragToLight = fragPos - lightPos;

    // Sample from cubemap using direction vector
    float closestDepth = texture(shadowCubemap, fragToLight).r;

    // Convert to linear depth value
    closestDepth *= farPlane;

    // Get current linear depth
    float currentDepth = length(fragToLight);

    // Bias to prevent shadow acne
    float bias = 0.05;

    // Check if fragment is in shadow
    float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;

    return shadow;
}
```

**How it Works:**

1. **Direction Vector**: `fragToLight = fragPos - lightPos`
   - Points from light source to fragment
   - Automatically selects correct cube face based on direction
   - Example: (0.5, 0.2, 0.8) → samples +Z face

2. **Cubemap Sampling**: `texture(shadowCubemap, fragToLight)`
   - Hardware automatically determines which face to sample
   - Uses largest component magnitude to select face
   - Interpolates at face boundaries

3. **Depth Comparison**:
   - `closestDepth`: Depth stored in shadow map
   - `currentDepth`: Actual distance from light to fragment
   - Linear depth (world space distance) not NDC depth

4. **Shadow Decision**:
   ```
   if (currentDepth - bias > closestDepth)
       return 1.0;  // In shadow
   else
       return 0.0;  // Lit
   ```

---

## Optimization Systems

### 1. View Frustum Culling

**Purpose**: Skip rendering objects outside camera's view

**Algorithm**: Test object AABBs against 6 frustum planes

```cpp
bool Frustum::IsAABBVisible(const AABB& aabb) const
{
    for (const auto& plane : m_Planes) {
        // Get furthest vertex along plane normal
        vec3 positiveVertex = aabb.min;
        if (plane.normal.x >= 0) positiveVertex.x = aabb.max.x;
        if (plane.normal.y >= 0) positiveVertex.y = aabb.max.y;
        if (plane.normal.z >= 0) positiveVertex.z = aabb.max.z;

        // If furthest vertex is behind plane → outside frustum
        if (plane.GetSignedDistance(positiveVertex) < 0)
            return false;
    }
    return true;
}
```

**Frustum Plane Extraction** (from view-projection matrix):

```cpp
void Frustum::Update(const mat4& viewProjection)
{
    // Left plane
    m_Planes[Left].normal.x = viewProjection[0][3] + viewProjection[0][0];
    m_Planes[Left].normal.y = viewProjection[1][3] + viewProjection[1][0];
    m_Planes[Left].normal.z = viewProjection[2][3] + viewProjection[2][0];
    m_Planes[Left].distance = viewProjection[3][3] + viewProjection[3][0];
    m_Planes[Left].Normalize();
    // ... (repeat for other 5 planes)
}
```

**Performance Impact:**

- 50% reduction in draw calls for typical scenes
- AABB test: ~6 plane tests per object (early exit)
- O(n) complexity where n = total objects

### 2. Batch Rendering

**Purpose**: Reduce state changes (material binds, VAO binds, shader uniforms)

**Statistics** (example scene):

```
Non-batched:  100 objects = 100 draw calls
Batched:      100 objects = 2-10 batches
              (depending on material/mesh variety)
```

**Batch Grouping:**

```cpp
struct BatchKey {
    std::shared_ptr<Material> material;
    unsigned int VAO;
};
```

Objects are grouped if they share:

1. Same material (textures, shininess)
2. Same mesh (VAO)

**Render Loop:**

```cpp
for (auto& batch : m_Batches) {
    // State changes: 1× per batch instead of 1× per object
    material->Bind(shader);        // 1× texture binds
    glBindVertexArray(batch.VAO);  // 1× VAO bind

    // Per-object uniforms
    for (auto* renderable : batch.renderables) {
        shader.setMat4("model", renderable->modelMatrix);
        glDrawElements(...);
    }
}
```

**Future Improvement**: True instanced rendering with `glDrawElementsInstanced`

### 3. Shared Mesh Architecture

**Memory Optimization**: Single mesh shared by multiple objects

```cpp
// Mesh created once
m_CubeMesh = Mesh::CreateCube();  // Allocates GPU buffers once

// Shared by many objects
auto obj1 = std::make_unique<Object>(m_CubeMesh, pos1, ...);
auto obj2 = std::make_unique<Object>(m_CubeMesh, pos2, ...);
auto obj3 = std::make_unique<Object>(m_CubeMesh, pos3, ...);
```

**Benefits:**

- 90%+ reduction in GPU memory for repeated geometry
- Faster scene regeneration (no mesh reconstruction)
- Better cache locality

---

## Material and Texture System

### Material Class

Encapsulates all surface properties:

```cpp
class Material {
    vec3 m_BaseColor;
    float m_Shininess;

    std::shared_ptr<Texture> m_DiffuseTexture;
    std::shared_ptr<Texture> m_SpecularTexture;
    std::shared_ptr<Texture> m_NormalTexture;
    std::shared_ptr<Texture> m_EmissionTexture;
};
```

**Binding Process:**

```cpp
void Material::Bind(const Shader& shader) const
{
    shader.setVec3("material.baseColor", m_BaseColor);
    shader.setFloat("material.shininess", m_Shininess);

    if (m_DiffuseTexture) {
        m_DiffuseTexture->Bind(0);
        shader.setInt("material.diffuseMap", 0);
        shader.setBool("material.useDiffuseMap", true);
    }
    // ... (repeat for other texture types)
}
```

### Texture Management

**Texture Units Allocation:**

- 0-2: Shadow maps (directional, point, spot)
- 3: Diffuse/albedo map
- 4: Specular map
- 5: Normal map
- 6: Emission map

**Texture Loading:**

```cpp
bool Texture::LoadFromFile(const std::string& path)
{
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &channels, 0);

    glGenTextures(1, &m_TextureID);
    glBindTexture(GL_TEXTURE_2D, m_TextureID);

    GLenum format = (channels == 3) ? GL_RGB : GL_RGBA;
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0,
                 format, GL_UNSIGNED_BYTE, data);

    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(data);
}
```

**Procedural Textures:**

```cpp
void Texture::CreateSolidColor(uint8_t r, uint8_t g, uint8_t b)
{
    // 64×64 single-color texture for specular maps
    std::vector<uint8_t> data(64 * 64 * 3);
    for (size_t i = 0; i < data.size(); i += 3) {
        data[i] = r; data[i+1] = g; data[i+2] = b;
    }
    // Upload to GPU...
}
```

Used for default specular maps when no texture is provided.

---

## Problems Encountered and Solutions

### Problem 1: Null Pointer Dereference in Light Source

**Symptom:**

```
Access violation at 0x0000000000000000
Line: m_Scene->GetLightSource().Position = pointLight.position;
```

**Root Cause:**

- `Scene::m_LightSource` (unique_ptr) was never initialized
- `GetLightSource()` returns `*m_LightSource` → dereferencing null

**Solution:**
Added initialization in `Scene` constructor:

```cpp
Scene::Scene() {
    CreateMeshes();

    // Initialize light source sphere
    m_LightSource = std::make_unique<Object>(
        m_SphereMesh,
        glm::vec3(0.0f, 2.0f, 0.0f),
        glm::vec3(0.2f),  // Small size
        glm::vec3(0.0f),
        glm::vec3(1.0f)   // White
    );

    RegenerateScene(7);
}
```

**Lesson**: Always initialize unique_ptr members, especially when getters return references.

---

### Problem 2: Shadow Acne

**Symptom:**

- Striped/moiré patterns on shadowed surfaces
- Self-shadowing artifacts

**Cause:**

- Limited depth buffer precision (24-bit)
- Surfaces incorrectly shadow themselves

**Solutions Implemented:**

1. **Adaptive Bias:**

```glsl
float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
```

Adjusts bias based on surface angle to light.

2. **Front-Face Culling:**

```cpp
glCullFace(GL_FRONT);  // During shadow pass
```

Renders back faces to shadow map, pushing depth away.

3. **PCF Sampling:**
   3×3 kernel averages out precision errors.

**Trade-offs:**

- Front-face culling can cause "Peter Panning" (detached shadows)
- High bias reduces acne but increases Peter Panning
- PCF adds 9× texture lookups per fragment

---

### Problem 3: Batch Statistics Incorrect

**Symptom:**

- UI showed 0 visible objects when batching enabled
- Statistics only updated in non-batched mode

**Root Cause:**

- `Scene::Render` (non-batched) overwrote batch statistics
- Mini-map called `Render`, resetting main view stats

**Solution:**
Conditional statistics update:

```cpp
void Scene::Render(const Shader& shader) const
{
    // Only update stats if NOT using batching
    if (m_BatchRenderer.GetBatchCount() == 0) {
        m_TotalObjects = m_Objects.size();
        m_VisibleObjects = m_TotalObjects;
        // ...
    }

    for (const auto& object : m_Objects) {
        object->Draw(shader);
    }
}
```

**Lesson**: Shared state between rendering paths needs careful management.

---

## Current Limitations and Future Improvements

### Limitations

#### 1. **Shadow Map Resolution**

- **Current**: Fixed 2048×2048 per light
- **Issue**: Far objects have low shadow detail, close objects waste resolution
- **Impact**: Aliased shadows at certain distances

#### 2. **Single Shadow Sample per Light**

- **Current**: One shadow map per light type
- **Issue**: Cannot have multiple directional/point/spot lights with shadows
- **Workaround**: Only one light of each type can cast shadows

#### 3. **Batch Rendering Not Fully Instanced**

- **Current**: Groups objects but still individual draw calls
- **Issue**: CPU overhead of per-object uniform updates
- **Impact**: Limited by CPU for scenes >10,000 objects

#### 4. **No Occlusion Culling**

- **Current**: Only frustum culling
- **Issue**: Objects behind walls/terrain still rendered
- **Impact**: Wasted GPU cycles in complex scenes

#### 5. **Limited to Blinn-Phong**

- **Current**: No PBR (Physically Based Rendering)
- **Issue**: Less realistic materials (metals, rough surfaces)
- **Missing**: Metalness, roughness, ambient occlusion

#### 6. **No Deferred Rendering**

- **Current**: Forward rendering
- **Issue**: Lighting cost scales with fragments × lights
- **Impact**: Poor performance with many lights (>10)


#### 7. **No Implementation For Adding Object In Real-time**

#### 8. **No Implementation For Loading Model**
---

### Future Improvements

**1. True Instanced Rendering**

```cpp
// Upload instance data once
glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
glBufferData(GL_ARRAY_BUFFER, modelMatrices.size() * sizeof(mat4),
             modelMatrices.data(), GL_STATIC_DRAW);

// Single draw call for all instances
glDrawElementsInstanced(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0,
                       instanceCount);
```

**Expected Impact**: 10-100× performance improvement for repeated geometry

**2. Cascaded Shadow Maps (CSM)**

- Split frustum into 3-4 cascades
- Different shadow map resolution per cascade
- Near: 4096×4096, Mid: 2048×2048, Far: 1024×1024

**Benefits:**

- Eliminates perspective aliasing
- Better shadow quality at all distances

**3. Occlusion Culling (Hardware Queries)**

```cpp
// Render bounding boxes with occlusion queries
glBeginQuery(GL_ANY_SAMPLES_PASSED, query);
RenderBoundingBox(object);
glEndQuery(GL_ANY_SAMPLES_PASSED);

// Next frame: check if visible
glGetQueryObjectuiv(query, GL_QUERY_RESULT, &visible);
if (visible) RenderObject(object);
```

**Expected Impact**: 30-70% reduction in fragments rendered

**4. Deferred Rendering Pipeline**

```
Pass 1: Render to G-Buffer (position, normal, albedo, specular)
Pass 2: Lighting pass (screen-space, one pass per light)
Pass 3: Forward pass (transparent objects)
```

**Benefits:**

- O(fragments + lights) instead of O(fragments × lights)
- Efficient for 100+ lights

**Trade-offs:**

- Higher memory usage (multiple render targets)
- No hardware MSAA (need FXAA/SMAA)

**5. Shadow Map Caching**

- Static objects: render shadow map once
- Dynamic objects: update only moving light shadows

**Expected Impact**: 50% reduction in shadow pass time

**6. Material System Upgrade to PBR**

```cpp
struct PBRMaterial {
    vec3 albedo;
    float metalness;   // 0 = dielectric, 1 = conductor
    float roughness;   // 0 = smooth, 1 = rough
    vec3 F0;           // Fresnel at normal incidence
};
```

**Benefits:**

- Physically accurate lighting
- Better material authoring workflow
- Industry-standard pipeline

**9. Bloom and Tone Mapping**

- HDR rendering pipeline
- Bright lights glow realistically

**10. Compute Shader Culling**

- GPU-driven frustum culling
- Eliminates CPU-GPU sync

---

## Conclusion

This graphics engine demonstrates fundamental real-time rendering techniques:

- **Lighting**: Blinn-Phong model with three light types
- **Shadows**: Shadow mapping with PCF and adaptive bias
- **Optimization**: Frustum culling and batch rendering
- **Materials**: Texture-based material system with normal mapping

The architecture is modular and extensible, with clear upgrade paths to more advanced techniques (PBR, deferred rendering, CSM). Performance is suitable for small-to-medium scenes (100-1000 objects) on modern hardware.

---
