

#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texture_diffuse1;

void main()
{   
    FragColor = vec4(1.5f,1.5f,1.5f,1.0f)*texture(texture_diffuse1, TexCoords);
}

