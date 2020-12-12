#version 330 core

out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D screenTexture;

uniform int effectFlag;

float kernelBlur[9] = float[](
    1.0 / 16, 2.0 / 16, 1.0 / 16,
    2.0 / 16, 4.0 / 16, 2.0 / 16,
    1.0 / 16, 2.0 / 16, 1.0 / 16  
);

float kernelSharp[9] = float[](
    -1, -1, -1,
    -1,  9, -1,
    -1, -1, -1
);

const float offset = 1.0f / 300.0f;
// 0 - normal
// 1 - gray
// 2 - inverse
// 3 - blur
// 4 - sharp
// other - normal
void main()
{
    if (effectFlag == 0) {
        FragColor = texture(screenTexture, TexCoord);
    } else if (effectFlag == 1) {
        FragColor = texture(screenTexture, TexCoord);
        float average = 0.2126f * FragColor.r + 0.7152f * FragColor.g + 0.0722f * FragColor.b;
        FragColor = vec4(average, average, average, 1.0);
    } else if (effectFlag == 2) {
        FragColor = vec4(vec3(1.0 - texture(screenTexture, TexCoord)), 1.0);
    } else if (effectFlag == 3 || effectFlag == 4) {
        vec2 offsets[9] = vec2[](
            vec2(-offset,  offset), // верхний-левый
            vec2( 0.0f,    offset), // верхний-центральный
            vec2( offset,  offset), // верхний-правый
            vec2(-offset,  0.0f),   // центральный-левый
            vec2( 0.0f,    0.0f),   // центральный-центральный
            vec2( offset,  0.0f),   // центральный-правый
            vec2(-offset, -offset), // нижний-левый
            vec2( 0.0f,   -offset), // нижний-центральный
            vec2( offset, -offset)  // нижний-правый    
        );
 
    
        vec3 sampleTex[9];
        for(int i = 0; i < 9; i++) {
            sampleTex[i] = vec3(texture(screenTexture, TexCoord.st + offsets[i]));
        }
        vec3 color = vec3(0.0);
        if (effectFlag == 3) {
            for(int i = 0; i < 9; i++) {
                color += sampleTex[i] * kernelBlur[i];
            }
        } else {
            for(int i = 0; i < 9; i++) {
                color += sampleTex[i] * kernelSharp[i];
            }
        }
        
        FragColor = vec4(color, 1.0);
    } else {
        FragColor = texture(screenTexture, TexCoord);
    }
}
