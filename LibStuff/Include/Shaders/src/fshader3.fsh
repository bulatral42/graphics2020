#version 330 core

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

void main()
{
    float ambLight = 0.1f, specLight = 0.5f;
    
    vec3 normal = normalize(NormalCoord);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diffLight = max(0.0f, dot(normal, lightDir));
    
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflDir = reflect(-lightDir, normal);
    specLight = specLight * pow(max(dot(viewDir, reflDir), 0.0f), 32);

    vec3 resColor = (ambLight + diffLight + specLight) * lightColor * objectColor;
    FragColor = mix(texture(ourTexture1, TexCoord), 
                    texture(ourTexture2, TexCoord), 
                    mixValue) * vec4(resColor, 1.0);
}
