#version 330 core

out vec4 FragColor;

in vec3 FragPos;
uniform vec3 borderColor;

void main()
{ 
    float gamma = 2.2;
    FragColor = vec4(borderColor, 1.0f);
    //FragColor.rgb = pow(FragColor.rgb, vec3(1.0/gamma));
}
