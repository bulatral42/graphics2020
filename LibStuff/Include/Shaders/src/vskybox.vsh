#version 330 core
layout(location = 0) in vec3 pos;

out vec3 TexCoord;

uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * vec4(pos, 1.0f);
    
    
    TexCoord = pos;// vec2(texCoord.x, 1.0f - texCoord.y);
}
