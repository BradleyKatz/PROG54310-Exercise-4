#version 330 core
out vec4 FragColor;

struct Material
{
	sampler2D diffuse;
	sampler2D specular;
	vec3 ka;
	vec3 kd;
	vec3 ks;
	float shininess;
	float alpha;
};

struct DirectionalLight
{
	vec3 direction;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

struct PointLight
{
	vec3 position;

	float constant;
	float linear;
	float quadratic;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

struct SpotLight
{
	vec3 position;
	vec3 direction;
	float innerCutoff;
	float outerCutoff;

	float constant;
	float linear;
	float quadratic;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

#define MAX_POINT_LIGHTS 36
#define MAX_SPOT_LIGHTS 4

in vec3 FragPos_WorldSpace;
in vec3 Normal;
in vec2 UV;

uniform float ambientLightStrength;
uniform vec3 viewPosition;
uniform DirectionalLight dirLight;
uniform int numPointLights;
uniform int numSpotLights;
uniform PointLight pointLights[MAX_POINT_LIGHTS];
uniform SpotLight spotLights[MAX_SPOT_LIGHTS];
uniform Material material;

vec3 CalcDirLight(DirectionalLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main()
{
	if (texture(material.diffuse, UV).a < 0.1f)
	{
		discard;
	}

	vec3 norm = normalize(Normal);
	vec3 viewDir = normalize(viewPosition - FragPos_WorldSpace);

	// Directional light = sun-like light source
	vec3 result = CalcDirLight(dirLight, norm, viewDir);

	for (int i = 0; i < numPointLights; i++)
	{
		result += CalcPointLight(pointLights[i], norm, FragPos_WorldSpace, viewDir);
	}

	for (int i = 0; i < numSpotLights; i++)
	{
		result += CalcSpotLight(spotLights[i], norm, FragPos_WorldSpace, viewDir);
	}

	FragColor = vec4(result, 1.0f) * vec4(material.ka * material.kd * material.ks, material.alpha);
}

vec3 CalcDirLight(DirectionalLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    
	// diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    
	// specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    
	// combine results
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, UV));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, UV));
    vec3 specular = light.specular * spec * vec3(texture(material.specular, UV));
    
	return (ambient + diffuse + specular);
//	return (ambient + diffuse);
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    
	// diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    
	// specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    
	// attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    
	// combine results
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, UV));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, UV));
    vec3 specular = light.specular * spec * vec3(texture(material.specular, UV));
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    
	return (ambient + diffuse + specular);
//	return (ambient + diffuse);
}

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    
	// diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    
	// specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    
	// attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    
	// spotlight intensity
    float theta = dot(lightDir, normalize(-light.direction)); 
    float epsilon = light.innerCutoff - light.outerCutoff;
    float intensity = clamp((theta - light.outerCutoff) / epsilon, 0.0, 1.0);
    
	// combine results
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, UV));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, UV));
    vec3 specular = light.specular * spec * vec3(texture(material.specular, UV));
    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;
    
	return (ambient + diffuse + specular);
//	return (ambient + diffuse);
}