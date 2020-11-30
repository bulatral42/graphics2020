#version 330 core

in vec3 ourColor;
in vec2 TexCoord;

out vec4 color;

uniform float mixValue;

uniform sampler2D ourTexture1;
uniform sampler2D ourTexture2;

void main()
{
    color = mix(texture(ourTexture1, TexCoord), texture(ourTexture2, TexCoord), mixValue) * vec4(ourColor, 1.0f);
}