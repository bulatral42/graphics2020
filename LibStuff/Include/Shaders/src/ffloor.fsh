#version 330 core

in vec3 ourColor;
in vec2 TexCoord;

out vec4 FragColor;

uniform sampler2D floorMap;

void main()
{
    FragColor = texture(floorMap, TexCoord);
}
