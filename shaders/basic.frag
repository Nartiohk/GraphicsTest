#version 330 core

in vec3 Normal;
in vec3 FragPos;

out vec4 FragColor;

uniform vec3 objectColor;
uniform vec3 viewPos;

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


vec3 CalculateBlinnPhong(
    vec3 lightDir,
    vec3 normal,
    vec3 viewDir,
    vec3 ambientColor,
    vec3 diffuseColor,
    vec3 specularColor,
    vec3 albedo,
    float shininess
)
{
    // Diffuse
    float diff = max(dot(normal, lightDir), 0.0);

    // Blinn-Phong Specular
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), shininess);

    vec3 ambient  = ambientColor * albedo;
    vec3 diffuse  = diffuseColor * diff * albedo;
    vec3 specular = specularColor * spec;

    return ambient + diffuse + specular;
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
    float shininess
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
        shininess
    );
}

// Calculates point light
vec3 CalcPointLight(
    PointLight light,
    vec3 normal,
    vec3 fragPos,
    vec3 viewDir,
    vec3 albedo,
    float shininess
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
        shininess
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
    float shininess
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
        shininess
    );

    return result * attenuation * intensity;
}


void main()
{
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 albedo = objectColor;  // your uniform

    float shininess = 32.0;

    vec3 result = vec3(0.0);

    result += CalcDirLight(dirLight, norm, viewDir, albedo, shininess);
    result += CalcPointLight(pointLight, norm, FragPos, viewDir, albedo, shininess);
    result += CalcSpotLight(spotLight, norm, FragPos, viewDir, albedo, shininess);

    FragColor = vec4(result, 1.0);
    // FragColor = vec4(vec3(gl_FragCoord.z), 1.0);
}

