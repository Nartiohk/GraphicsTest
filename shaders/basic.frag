#version 330 core

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoord;
in mat3 TBN;

out vec4 FragColor;

uniform vec3 objectColor;
uniform vec3 viewPos;

// Material system
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

// Legacy texture support (for backward compatibility)
uniform bool useTexture;
uniform sampler2D objectTexture;

uniform bool enableShadows;
uniform bool enableDirLight;
uniform bool enablePointLight;
uniform bool enableSpotLight;
uniform bool useNormalMapping; // Global toggle for normal mapping

// Shadow maps
uniform sampler2D dirLightShadowMap;
uniform sampler2D pointLightShadowMap;
uniform sampler2D spotLightShadowMap;

// Light space matrices
uniform mat4 dirLightSpaceMatrix;
uniform mat4 pointLightSpaceMatrix;
uniform mat4 spotLightSpaceMatrix;

// Directional Light
struct DirLight {
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform DirLight dirLight;

// Point Light
struct PointLight {
    vec3 position;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform PointLight pointLight;

// Spot Light
struct SpotLight {
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform SpotLight spotLight;

// Function prototypes
float ShadowCalculation(vec4 fragPosLightSpace, sampler2D shadowMap, vec3 normal, vec3 lightDir);


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

    // Apply shadow (ambient is not affected)
    return ambient + (1.0 - shadow) * (diffuse + specular);
}

float CalculateAttenuation(
    float constant,
    float linear,
    float quadratic,
    float distance
)
{
    return 1.0 / (constant + linear * distance + quadratic * distance * distance);
}

float CalculateSpotIntensity(
    vec3 lightDir,
    vec3 spotDirection,
    float cutOff,
    float outerCutOff
)
{
    float theta = dot(lightDir, normalize(-spotDirection));
    float epsilon = cutOff - outerCutOff;

    return clamp((theta - outerCutOff) / epsilon, 0.0, 1.0);
}

// Calculates directional light
vec3 CalcDirLight(
    DirLight light,
    vec3 normal,
    vec3 viewDir,
    vec3 albedo,
    float shininess,
    float specularIntensity,
    float shadow
)
{
    vec3 lightDir = normalize(-light.direction);

    return CalculateBlinnPhong(
        lightDir,
        normal,
        viewDir,
        light.ambient,
        light.diffuse,
        light.specular,
        albedo,
        shininess,
        specularIntensity,
        shadow
    );
}

// Calculates point light
vec3 CalcPointLight(
    PointLight light,
    vec3 normal,
    vec3 fragPos,
    vec3 viewDir,
    vec3 albedo,
    float shininess,
    float specularIntensity,
    float shadow
)
{
    vec3 lightDir = normalize(light.position - fragPos);
    float distance = length(light.position - fragPos);

    float attenuation = CalculateAttenuation(
        light.constant,
        light.linear,
        light.quadratic,
        distance
    );

    vec3 result = CalculateBlinnPhong(
        lightDir,
        normal,
        viewDir,
        light.ambient,
        light.diffuse,
        light.specular,
        albedo,
        shininess,
        specularIntensity,
        shadow
    );

    return result * attenuation;
}

// Calculates spot light
vec3 CalcSpotLight(
    SpotLight light,
    vec3 normal,
    vec3 fragPos,
    vec3 viewDir,
    vec3 albedo,
    float shininess,
    float specularIntensity,
    float shadow
)
{
    vec3 lightDir = normalize(light.position - fragPos);
    float distance = length(light.position - fragPos);

    float attenuation = CalculateAttenuation(
        light.constant,
        light.linear,
        light.quadratic,
        distance
    );

    float intensity = CalculateSpotIntensity(
        lightDir,
        light.direction,
        light.cutOff,
        light.outerCutOff
    );

    vec3 result = CalculateBlinnPhong(
        lightDir,
        normal,
        viewDir,
        light.ambient,
        light.diffuse,
        light.specular,
        albedo,
        shininess,
        specularIntensity,
        shadow
    );

    return result * attenuation * intensity;
}

// Shadow calculation with PCF (Percentage Closer Filtering)
float ShadowCalculation(vec4 fragPosLightSpace, sampler2D shadowMap, vec3 normal, vec3 lightDir)
{
    // Perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;

    // Transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;

    // Check if outside shadow map bounds
    if (projCoords.z > 1.0)
        return 0.0;

    // Get current depth
    float currentDepth = projCoords.z;

    // Bias to prevent shadow acne
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);

    // PCF (Percentage Closer Filtering) for softer shadows
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;

    return shadow;
}

void main()
{
    // Get normal from normal map or use vertex normal
    vec3 norm;
    if (useNormalMapping && material.useNormalMap)
    {
        // Sample normal from normal map
        norm = texture(material.normalMap, TexCoord).rgb;
        // Transform from [0,1] to [-1,1]
        norm = normalize(norm * 2.0 - 1.0);
        // Transform to world space using TBN matrix
        norm = normalize(TBN * norm);
    }
    else
    {
        norm = normalize(Normal);
    }

    vec3 viewDir = normalize(viewPos - FragPos);

    // Get albedo (diffuse color)
    vec3 albedo;
    if (material.useDiffuseMap)
    {
        albedo = texture(material.diffuseMap, TexCoord).rgb;
    }
    else if (useTexture) // Legacy texture support
    {
        albedo = texture(objectTexture, TexCoord).rgb;
    }
    else
    {
        albedo = objectColor * material.baseColor;
    }

    // Get specular intensity
    float specularIntensity;
    if (material.useSpecularMap)
    {
        // Specular map is usually grayscale, so we can use any channel
        specularIntensity = texture(material.specularMap, TexCoord).r;
    }
    else
    {
        specularIntensity = 1.0; // Default full specular
    }

    float shininess = material.shininess;

    vec3 result = vec3(0.0);

    // Directional light
    if (enableDirLight)
    {
        float shadow = 0.0;
        if (enableShadows)
        {
            vec4 fragPosLightSpace = dirLightSpaceMatrix * vec4(FragPos, 1.0);
            vec3 lightDir = normalize(-dirLight.direction);
            shadow = ShadowCalculation(fragPosLightSpace, dirLightShadowMap, norm, lightDir);
        }
        result += CalcDirLight(dirLight, norm, viewDir, albedo, shininess, specularIntensity, shadow);
    }

    // Point light
    if (enablePointLight)
    {
        float shadow = 0.0;
        if (enableShadows)
        {
            vec4 fragPosLightSpace = pointLightSpaceMatrix * vec4(FragPos, 1.0);
            vec3 lightDir = normalize(pointLight.position - FragPos);
            shadow = ShadowCalculation(fragPosLightSpace, pointLightShadowMap, norm, lightDir);
        }
        result += CalcPointLight(pointLight, norm, FragPos, viewDir, albedo, shininess, specularIntensity, shadow);
    }

    // Spot light
    if (enableSpotLight)
    {
        float shadow = 0.0;
        if (enableShadows)
        {
            vec4 fragPosLightSpace = spotLightSpaceMatrix * vec4(FragPos, 1.0);
            vec3 lightDir = normalize(spotLight.position - FragPos);
            shadow = ShadowCalculation(fragPosLightSpace, spotLightShadowMap, norm, lightDir);
        }
        result += CalcSpotLight(spotLight, norm, FragPos, viewDir, albedo, shininess, specularIntensity, shadow);
    }

    // Add emission if available
    if (material.useEmissionMap)
    {
        vec3 emission = texture(material.emissionMap, TexCoord).rgb;
        result += emission;
    }

    // If no lights are enabled, use minimal ambient
    if (!enableDirLight && !enablePointLight && !enableSpotLight)
    {
        result = albedo * 0.1;
    }

    FragColor = vec4(result, 1.0);
}

