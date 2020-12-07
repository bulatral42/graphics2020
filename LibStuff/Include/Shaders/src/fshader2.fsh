#version 330 core

out vec4 FragColor;

in vec3 objectColor;
in vec2 TexCoord;

uniform sampler2D ourTexture1;
uniform sampler2D ourTexture2;

uniform float mixValue;

uniform vec3 lightColor;

void main()
{
    FragColor = mix(texture(ourTexture1, TexCoord), 
                    texture(ourTexture2, TexCoord), 
                    mixValue) * 
                    vec4(lightColor * objectColor, 1.0);
}
