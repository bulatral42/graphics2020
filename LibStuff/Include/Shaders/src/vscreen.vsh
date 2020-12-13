#version 330 core
layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 texCoord;

out vec2 TexCoord;

void main()
{
    gl_Position = vec4(pos.x, pos.y, 0.0f, 1.0f);
    TexCoord = texCoord; //TexCoord = vec2(texCoord.x, 1.0f - texCoord.y);
}
