#version 330 core

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    sampler2D emission;
    float shininess;
};

struct Light {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

out vec4 FragColor;

in vec3 objectColor;
in vec2 TexCoord;
in vec3 NormalCoord;
in vec3 FragPos;


uniform vec3 lightPos;
uniform vec3 lightColor;

uniform vec3 viewPos;

uniform float curTime;

uniform Material material;
uniform Light light;

void main()
{
    vec3 textureDiffuse = vec3(texture(material.diffuse, TexCoord));
    vec3 textureSpecular = vec3(texture(material.specular, TexCoord));
    
    vec3 ambLight = textureDiffuse * light.ambient;
    
    vec3 normal = normalize(NormalCoord);
    vec3 lightDir = normalize(light.position - FragPos);
    vec3 diffLight = max(0.0f, dot(normal, lightDir)) * 
                     textureDiffuse * light.diffuse;
    
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflDir = reflect(-lightDir, normal);
    vec3 specLight = pow(max(dot(viewDir, reflDir), 0.0f), 
                         material.shininess) *
                         textureSpecular * light.specular;
    
    vec3 emissLight = vec3(0.0f);
    if (texture(material.specular, TexCoord).r < 0.1) {
        emissLight = (0.5f + 0.5f * sin(0.5 * curTime)) * vec3(texture(material.emission, TexCoord));
    }
    vec3 resColor = (ambLight + diffLight + specLight + emissLight); //* objectColor;

    FragColor = vec4(resColor, 1.0);
}
