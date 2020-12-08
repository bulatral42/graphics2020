#version 330 core

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
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

uniform sampler2D ourTexture1;
uniform sampler2D ourTexture2;

uniform float mixValue;

uniform vec3 lightPos;
uniform vec3 lightColor;

uniform vec3 viewPos;

uniform Material material;
uniform Light light;

void main()
{
    vec3 ambLight = material.ambient * light.ambient;
    
    vec3 normal = normalize(NormalCoord);
    vec3 lightDir = normalize(light.position - FragPos);
    vec3 diffLight = max(0.0f, dot(normal, lightDir)) * 
                     material.diffuse * light.diffuse;
    
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflDir = reflect(-lightDir, normal);
    vec3 specLight = pow(max(dot(viewDir, reflDir), 0.0f), 
                         material.shininess) *
                         material.specular * light.specular;

    vec3 resColor = (ambLight + diffLight + specLight);// * lightColor;// * objectColor;

    FragColor = mix(texture(ourTexture1, TexCoord), 
                    texture(ourTexture2, TexCoord), 
                    mixValue) * vec4(resColor, 1.0);
    //FragColor = vec4(resColor, 1.0);
}
