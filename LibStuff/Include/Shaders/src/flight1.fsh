#version 330 core

out vec4 FragColor;

uniform vec3 lightColor;

void main()
{
    float gamma = 2.2;
    FragColor = vec4(lightColor, 1.0f);
    //FragColor.rgb = pow(FragColor.rgb, vec3(1.0/gamma));
}