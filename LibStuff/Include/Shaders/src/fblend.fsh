#version 330 core
out vec4 FragColor;

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    sampler2D emission;
    float shininess;
};

in vec2 TexCoord;
 
uniform Material material;
 
void main()
{             
    FragColor = texture(material.diffuse, TexCoord);
}  

/*#version 330 core

out vec4 FragColor;

in vec3 objectColor;
in vec2 TexCoord;
in vec3 NormalCoord;
in vec3 FragPos;

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

vec4 calcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec4 calcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir); 

uniform DirLight dirLight;
uniform PointLight pointLights[NR_POINT_LIGHTS];

uniform vec3 viewPos;

uniform float curTime;

uniform Material material;

//float near = 0.1f; 
//float far  = 100.0f; 
  
//float LinearizeDepth(float depth) 
//{
//    float z = depth * 2.0f - 1.0f;
//    return (2.0f * near * far) / (far + near - z * (far - near));	
//}

void main()
{
    vec3 normal = normalize(NormalCoord);
    vec3 viewDir = normalize(viewPos - FragPos);

    vec4 resLight = calcDirLight(dirLight, normal, viewDir);

    for (int i = 0; i < NR_POINT_LIGHTS; ++i) {
        resLight += calcPointLight(pointLights[i], normal, FragPos, viewDir); 
    }

    //if (texture(material.specular, TexCoord).r < 0.1) {
    //    resLight += (0.5f + 0.5f * sin(0.5 * curTime)) * vec3(texture(material.emission, TexCoord)); // emissLight
    //}

    FragColor = resLight;
}

vec4 calcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec4 textureDiffuse = texture(material.diffuse, TexCoord);
    vec4 textureSpecular = texture(material.specular, TexCoord);
    vec3 lightDir = normalize(-light.direction);
    vec3 reflDir = reflect(-lightDir, normal);

    vec4 ambLight  = textureDiffuse * vec4(light.ambient, 1.0f);
    vec4 diffLight = max(0.0f, dot(normal, lightDir)) * textureDiffuse * vec4(light.diffuse, 1.0f);
    vec4 specLight = pow(max(0.0f, dot(viewDir, reflDir)), material.shininess) * textureSpecular * vec4(light.specular, 1.0f);

    return ambLight + diffLight + specLight;
}

vec4 calcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
	float distance = length(light.position - fragPos);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
	DirLight asDirLight;
	asDirLight.direction = fragPos - light.position,
	asDirLight.ambient = light.ambient,
	asDirLight.diffuse = light.diffuse,
	asDirLight.specular = light.specular;

	vec4 tmpLight = calcDirLight(asDirLight, normal, viewDir);
	return attenuation * tmpLight;
}
*/