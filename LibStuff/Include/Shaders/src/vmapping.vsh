#version 330 core
layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 texCoord;
layout(location = 2) in vec3 normalCoord;
layout(location = 3) in vec3 Tangent;
layout(location = 4) in vec3 Bitangent;

out VSH_OUT {
    vec3 FragPos;
    vec2 TexCoord;
    vec3 TangentLightPos;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
    mat3 TBN;
} vsh_out;


//out vec3 NormalCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec3 lightPos;
uniform vec3 viewPos;


void main()
{
    vsh_out.FragPos = vec3(model * vec4(pos, 1.0));
    vsh_out.TexCoord = texCoord;//vec2(texCoord.y, texCoord.x);

    mat3 normalMatrix = transpose(inverse(mat3(model)));
    vec3 T = normalize(normalMatrix * Tangent);
    vec3 N = normalize(normalMatrix * normalCoord);
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T);

    mat3 TBN = transpose(mat3(T, B, N));
    vsh_out.TangentLightPos = TBN * lightPos;
    vsh_out.TangentViewPos = TBN * viewPos;
    vsh_out.TangentFragPos = TBN * vsh_out.FragPos;
    vsh_out.TBN = TBN;

    gl_Position = projection * view * model * vec4(pos, 1.0);
}
