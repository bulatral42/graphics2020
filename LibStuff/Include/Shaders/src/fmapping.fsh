#version 330 core

out vec4 FragColor;


//in vec2 TexCoord;
//in vec3 NormalCoord;
//in vec3 FragPos;

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

uniform DirLight dirLight;
uniform PointLight pointLights[NR_POINT_LIGHTS];

uniform vec3 viewPos;

uniform float curTime;

uniform Material material;

uniform sampler2D normalMap;
uniform sampler2D depthMap;
uniform float height_scale;

//float near = 0.1f; 
//float far  = 100.0f; 
  
//float LinearizeDepth(float depth) 
//{
//    float z = depth * 2.0f - 1.0f;
//    return (2.0f * near * far) / (far + near - z * (far - near));	
//}
vec2 newTexCoord;
//float ldp;

void main()
{
    vec3 viewDir = normalize(fsh_in.TangentViewPos - fsh_in.TangentFragPos);
    newTexCoord = reliefPM(fsh_in.TexCoord,  viewDir);
    vec3 normal = texture(normalMap, newTexCoord).rgb;
    normal = normalize(normal * 2.0 - 1.0); // tangent space
    
    vec4 resLight = calcDirLight(dirLight, normal, viewDir);

    for (int i = 0; i < NR_POINT_LIGHTS; ++i) {
        resLight += calcPointLight(pointLights[i], normal, fsh_in.FragPos, viewDir); 
    }

    //if (texture(material.specular, TexCoord).r < 0.1) {
    //    resLight += (0.5f + 0.5f * sin(0.5 * curTime)) * vec3(texture(material.emission, newTexCoord)); // emissLight
    //}

    float gamma = 2.2;
    FragColor = resLight;
    //FragColor = vec4(normal, 1.0f);
    //FragColor.rgb = pow(FragColor.rgb, vec3(1.0 / gamma));
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
    vec4 specLight = pow(max(0.0f, dot(normal, halfwayDir/*reflDir*/)), material.shininess) * textureSpecular * vec4(light.specular, 1.0f);

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

float depthValue(vec2 texCoord)
{
    return texCoord.r;
}

vec2 reliefPM(vec2 texCoord, vec3 viewDir)
{
	const float minLayers = 2.0f;
	const float maxLayers = 32.0f;
	float numLayers = mix(maxLayers, minLayers, abs(dot(vec3(0.0f, 0.0f, 1.0f), viewDir)));

	float deltaDepth = 1.0f / numLayers;
	vec2 deltaTexCoord = height_scale * viewDir.xy / (viewDir.z * numLayers);

	vec2 curTexCoord = texCoord;
	float curLayerDepth = 0.;
	float curDepthValue = depthValue(curTexCoord);
	while (curDepthValue > curLayerDepth) {
		curLayerDepth += deltaDepth;
		curTexCoord -= deltaTexCoord;
		curDepthValue = depthValue(curTexCoord);
	}


	deltaTexCoord *= 0.5;
	deltaDepth *= 0.5;

	curTexCoord += deltaTexCoord;
	curLayerDepth -= deltaDepth;

	const int reliefSteps = 5;
	int curStep = reliefSteps;
	while (curStep > 0) {
		curDepthValue = depthValue(curTexCoord);
		deltaTexCoord *= 0.5;
		deltaDepth *= 0.5;

		if (curDepthValue > curLayerDepth) {
			curTexCoord -= deltaTexCoord;
			curLayerDepth += deltaDepth;
		}

		else {
			curTexCoord += deltaTexCoord;
			curLayerDepth -= deltaDepth;
		}
		curStep--;
	}

	//lastDepthValue = currentDepthValue;
	return curTexCoord;
}