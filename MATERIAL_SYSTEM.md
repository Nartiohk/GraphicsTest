# Advanced Material System - Normal, Diffuse, and Specular Mapping

## Overview
A complete material system has been implemented with support for:
- **Diffuse Mapping** - Base color textures
- **Normal Mapping** - Surface detail without additional geometry
- **Specular Mapping** - Variable shininess across surfaces
- **Emission Mapping** - Self-illuminating surfaces (ready for use)

## Features Implemented

### 1. Material Class (`Material.h/cpp`)
Central material management system that holds multiple texture types:

```cpp
auto material = std::make_shared<Material>();

// Set textures
material->SetDiffuseTexture(diffuseTexture);
material->SetNormalTexture(normalTexture);
material->SetSpecularTexture(specularTexture);
material->SetEmissionTexture(emissionTexture);

// Set properties
material->SetShininess(32.0f);
material->SetBaseColor(glm::vec3(1.0f, 1.0f, 1.0f));

// Bind automatically assigns all textures to appropriate units
material->Bind(shader);
```

### 2. Tangent Space Normal Mapping
**Vertex Shader Updates:**
- Added `aTangent` and `aBitangent` vertex attributes
- Calculate TBN (Tangent-Bitangent-Normal) matrix per vertex
- Transform normals from tangent space to world space

**Fragment Shader Updates:**
- Sample normal map in tangent space
- Transform from [0,1] to [-1,1] range
- Apply TBN matrix to get world-space normal
- Use transformed normal for all lighting calculations

**Benefits:**
- Surface detail without adding polygons
- Realistic bumps, ridges, and surface irregularities
- Works with all light types (directional, point, spot)

### 3. Specular Mapping
**Per-Pixel Specularity:**
- Grayscale specular maps control shininess per pixel
- Black (0.0) = No specularity (matte)
- White (1.0) = Full specularity (glossy)
- Gray values = Varying shininess levels

**Implementation:**
```glsl
float specularIntensity;
if (material.useSpecularMap)
{
    specularIntensity = texture(material.specularMap, TexCoord).r;
}
else
{
    specularIntensity = 1.0; // Default full specular
}
```

### 4. Updated Geometry
All geometry classes now include:
- **Tangent vectors** (for U direction in texture space)
- **Bitangent vectors** (for V direction in texture space)
- **14 floats per vertex**: Position(3) + Normal(3) + TexCoord(2) + Tangent(3) + Bitangent(3)

**Cube TBN Calculation:**
Each face has proper tangent/bitangent vectors calculated based on texture coordinates and face orientation.

**Plane TBN Calculation:**
Simple upward-facing plane with tangent along X-axis and bitangent along Z-axis.

### 5. Texture Unit Allocation

| Unit | Purpose |
|------|---------|
| 0 | Directional Light Shadow Map |
| 1 | Point Light Shadow Map |
| 2 | Spot Light Shadow Map |
| 3 | Diffuse Map |
| 4 | Specular Map |
| 5 | Normal Map |
| 6 | Emission Map |

## Shader System

### Material Struct in Fragment Shader
```glsl
struct Material {
    vec3 baseColor;
    float shininess;
    
    bool useDiffuseMap;
    sampler2D diffuseMap;
    
    bool useSpecularMap;
    sampler2D specularMap;
    
    bool useNormalMap;
    sampler2D normalMap;
    
    bool useEmissionMap;
    sampler2D emissionMap;
};
uniform Material material;
```

### Normal Mapping Flow
```glsl
vec3 norm;
if (material.useNormalMap)
{
    // Sample and transform normal
    norm = texture(material.normalMap, TexCoord).rgb;
    norm = normalize(norm * 2.0 - 1.0); // [0,1] to [-1,1]
    norm = normalize(TBN * norm); // Tangent to world space
}
else
{
    norm = normalize(Normal); // Use vertex normal
}
```

### Specular Mapping Flow
```glsl
float specularIntensity;
if (material.useSpecularMap)
{
    specularIntensity = texture(material.specularMap, TexCoord).r;
}
else
{
    specularIntensity = 1.0;
}

// Apply to specular calculation
vec3 specular = specularColor * spec * specularIntensity;
```

## Demo Materials

### Brick Material
```cpp
m_BrickMaterial = std::make_shared<Material>();

// Diffuse: Full color brick texture
m_BrickMaterial->SetDiffuseTexture(brickDiffuse);

// Normal: Detailed brick surface with depth
m_BrickMaterial->SetNormalTexture(brickNormal);

// Specular: Medium gray (semi-matte surface)
m_BrickMaterial->SetSpecularTexture(brickSpecular);
m_BrickMaterial->SetShininess(16.0f);
```

**Visual Effect:**
- Bricks appear to have depth and mortar lines
- Less shiny (realistic for rough bricks)
- Normal map creates realistic lighting variation

### Container Material
```cpp
m_ContainerMaterial = std::make_shared<Material>();

// Diffuse: Wood texture
m_ContainerMaterial->SetDiffuseTexture(containerDiffuse);

// Specular: Bright (shiny varnished wood)
m_ContainerMaterial->SetSpecularTexture(containerSpecular);
m_ContainerMaterial->SetShininess(64.0f);
```

**Visual Effect:**
- Wooden container texture
- Shiny highlights (varnished wood effect)
- No normal map (flat but specular)

## Usage Examples

### Creating a Material with All Maps
```cpp
// Load textures
auto diffuse = std::make_shared<Texture>();
diffuse->LoadFromFile("textures/material_diffuse.png");

auto normal = std::make_shared<Texture>();
normal->LoadFromFile("textures/material_normal.png");

auto specular = std::make_shared<Texture>();
specular->LoadFromFile("textures/material_specular.png");

// Create material
auto material = std::make_shared<Material>();
material->SetDiffuseTexture(diffuse);
material->SetNormalTexture(normal);
material->SetSpecularTexture(specular);
material->SetShininess(32.0f);

// Apply to object
cube->SetMaterial(material);
```

### Creating a Procedural Specular Map
```cpp
// High specularity (shiny metal)
auto shinySpec = std::make_shared<Texture>();
shinySpec->CreateSolidColor(255, 255, 255); // White = very shiny

// Low specularity (matte rubber)
auto matteSpec = std::make_shared<Texture>();
matteSpec->CreateSolidColor(32, 32, 32); // Dark gray = barely shiny

// Variable specularity
auto variedSpec = std::make_shared<Texture>();
variedSpec->CreateSolidColor(128, 128, 128); // Medium gray = medium shine
```

### Material Without Normal Map
```cpp
auto material = std::make_shared<Material>();
material->SetDiffuseTexture(diffuse);
material->SetSpecularTexture(specular);
// No normal map = uses vertex normals
material->SetShininess(32.0f);
```

## Technical Details

### TBN Matrix Calculation
For each vertex, we calculate:
```
T = Tangent vector (along U texture coordinate)
B = Bitangent vector (along V texture coordinate)
N = Normal vector (perpendicular to surface)

TBN = mat3(T, B, N)
```

The TBN matrix transforms from tangent space (where normal maps are defined) to world space (where lighting is calculated).

### Normal Map Format
- **RGB channels** represent XYZ normal direction
- **Red channel** = X component (left/right)
- **Green channel** = Y component (up/down)
- **Blue channel** = Z component (toward/away from surface)
- Typically bluish because most normals point "out" (Z+)

### Specular Map Format
- **Grayscale** image (can use any channel)
- **Black (0)** = No specular highlights
- **White (255)** = Full specular highlights
- **Gray values** = Interpolated specularity

## Performance Considerations

1. **Texture Memory**: Each material can use up to 4 textures
2. **Texture Sampling**: 1-3 texture samples per pixel (depending on maps used)
3. **TBN Calculation**: Computed per vertex, interpolated per pixel
4. **Conditional Branching**: Minimal impact due to uniform branching

**Optimization Tips:**
- Share materials between objects when possible
- Use texture atlases for small objects
- Compress textures to reduce memory
- Use mipmaps for distant objects

## Backward Compatibility

The system maintains full backward compatibility:
- Objects without materials render with solid colors
- `SetTexture()` method still works (creates material internally)
- Legacy `useTexture` uniform still supported

```cpp
// Old way (still works)
cube->SetTexture(texture);

// New way (recommended)
auto material = std::make_shared<Material>();
material->SetDiffuseTexture(texture);
cube->SetMaterial(material);
```

## Visual Results

When running the application, you will see:

**Cubes:**
- Alternating brick and container materials
- Brick cubes show normal-mapped surface detail
- Container cubes show specular wooden surface
- Lighting reacts realistically to both materials

**Plane (Ground):**
- Brick material with tiled normal mapping
- Surface appears to have real depth
- Lighting creates realistic shadows in mortar lines

**Sphere:**
- Solid color (no material assigned)
- Demonstrates contrast with textured objects

## Files Modified/Created

### New Files:
- `src/Material.h` - Material class header
- `src/Material.cpp` - Material implementation
- `textures/brickwall.jpg` - Brick diffuse texture
- `textures/brickwall_normal.jpg` - Brick normal map

### Modified Files:
- `shaders/basic.vert` - Added tangent/bitangent, TBN matrix
- `shaders/basic.frag` - Material system, normal/specular mapping
- `src/Cube.h/cpp` - Material support, tangent/bitangent vertices
- `src/Plane.h/cpp` - Material support, tangent/bitangent vertices
- `src/Scene.h/cpp` - Material creation and management
- `CMakeLists.txt` - Added Material.cpp

## Troubleshooting

### Normal mapping not visible
- Check that normal map is loaded correctly
- Verify TBN matrix calculation
- Ensure tangent/bitangent are correctly specified
- Check that normal map is in tangent space (RGB, not grayscale)

### Specular highlights wrong
- Verify specular map is grayscale
- Check shininess value (lower = larger highlights)
- Ensure lights are positioned to create highlights
- Check that specular intensity is being applied

### Textures appear black
- Verify all texture units are correct (3-6 for materials)
- Check that Material::Bind() is called before drawing
- Ensure textures are loaded successfully

## Future Enhancements

Possible additions:
1. **Parallax Occlusion Mapping** - Even more realistic depth
2. **Ambient Occlusion Maps** - Improved shadow in crevices
3. **Roughness Maps** - PBR-style roughness (instead of specular)
4. **Metalness Maps** - Full PBR material system
5. **Height Maps** - Displacement mapping
6. **Reflection Maps** - Environment reflections

---

## Quick Reference

```cpp
// Create full material
auto mat = std::make_shared<Material>();
mat->SetDiffuseTexture(diffuse);     // Color
mat->SetNormalTexture(normal);       // Surface detail
mat->SetSpecularTexture(specular);   // Shininess variation
mat->SetShininess(32.0f);            // Overall shininess

// Apply to object
object->SetMaterial(mat);

// The material system handles everything automatically!
```

**✅ Normal mapping working**  
**✅ Specular mapping working**  
**✅ Diffuse mapping working**  
**✅ Backward compatible**  
**✅ Build successful**

The advanced material system is fully operational and ready for use!
