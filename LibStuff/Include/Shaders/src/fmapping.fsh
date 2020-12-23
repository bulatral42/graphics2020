#version 330 core

out vec4 FragColor;


struct Material {
    sampler2D diffuse;
    sampler2D specular;
    sampler2D emission;
    float shininess;
};

struct DirLight {
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {    
    vec3 position;
    
    float constant;
    float linear;
    float quadratic;  
 
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

#define NR_POINT_LIGHTS 3

in VSH_OUT {
    vec3 FragPos;
    vec2 TexCoord;
    vec3 TangentLightPos;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
    mat3 TBN;
} fsh_in;

vec4 calcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec4 calcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir); 
vec2 reliefPM(vec2 texCoord, vec3 viewDir);
vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir);

uniform DirLight dirLight;
uniform PointLight pointLights[NR_POINT_LIGHTS];

uniform vec3 viewPos;

uniform float curTime;

uniform Material material;

uniform sampler2D normalMap;
uniform sampler2D depthMap;
uniform float heightScale;


vec2 newTexCoord;

void main()
{
    vec3 viewDir = normalize(fsh_in.TangentViewPos - fsh_in.TangentFragPos);
    newTexCoord = reliefPM(fsh_in.TexCoord,  viewDir);

    if (newTexCoord.x > 10.0 || newTexCoord.y > 10.0 || newTexCoord.x < 0.0 || newTexCoord.y < 0.0) {
        discard;
    }

    vec3 normal = texture(normalMap, newTexCoord).rgb;
    normal = normalize(normal * 2.0f - 1.0f); // tangent space
    
    vec4 resLight = calcDirLight(dirLight, normal, viewDir);

    for (int i = 0; i < NR_POINT_LIGHTS; ++i) {
        resLight += calcPointLight(pointLights[i], normal, fsh_in.FragPos, viewDir); 
    }

    FragColor = resLight;
}

vec4 calcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec4 textureDiffuse = texture(material.diffuse, newTexCoord);
    vec4 textureSpecular = texture(material.specular, newTexCoord);
    vec3 lightDir =  normalize(-fsh_in.TBN * light.direction);
    vec3 reflDir = reflect(-lightDir, normal);
    vec3 halfwayDir = normalize(lightDir + viewDir);

    vec4 ambLight  = textureDiffuse * vec4(light.ambient, 1.0f);
    vec4 diffLight = max(0.0f, dot(normal, lightDir)) * textureDiffuse * vec4(light.diffuse, 1.0f);
    vec4 specLight = pow(max(0.0f, dot(normal, halfwayDir)), material.shininess) * textureSpecular * vec4(light.specular, 1.0f);

    return ambLight + diffLight + specLight;
}

vec4 calcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
	float dist = length(light.position - fragPos);
	float attenuation = 1.0 / (light.constant + light.linear * dist + light.quadratic * (dist * dist));
	DirLight asDirLight;
	asDirLight.direction = fragPos - light.position,
	asDirLight.ambient = light.ambient,
	asDirLight.diffuse = light.diffuse,
	asDirLight.specular = light.specular;

	vec4 tmpLight = calcDirLight(asDirLight, normal, viewDir);
	return attenuation * tmpLight;
}

vec2 reliefPM(vec2 texCoord, vec3 viewDir)
{
	const float minLayers = 4.0f, maxLayers = 32.0f;
	float numLayers = mix(maxLayers, minLayers, abs(viewDir.z));

	float deltaDepth = 1.0f / numLayers;
	vec2 deltaTexCoord = heightScale * viewDir.xy / (viewDir.z * numLayers);

	vec2 curTexCoord = texCoord;
	float curLayerDepth = 0.0f;
	float curDepthValue = texture(depthMap, curTexCoord).r;
	while (curDepthValue > curLayerDepth) {
		curLayerDepth += deltaDepth;
		curTexCoord -= deltaTexCoord;
		curDepthValue = texture(depthMap, curTexCoord).r;
	}

	deltaTexCoord *= 0.5;
	deltaDepth *= 0.5;

	curTexCoord += deltaTexCoord;
	curLayerDepth -= deltaDepth;

    const int maxSteps = 7;
    for (int step = 0; step < maxSteps; ++step) {
        curDepthValue = texture(depthMap, curTexCoord).r;
		deltaTexCoord *= 0.5;
		deltaDepth *= 0.5;

		if (curDepthValue > curLayerDepth) {
			curTexCoord -= deltaTexCoord;
			curLayerDepth += deltaDepth;
		} else {
			curTexCoord += deltaTexCoord;
			curLayerDepth -= deltaDepth;
		}
    }

	return curTexCoord;
}
