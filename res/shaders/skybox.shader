#shader vertex
#version 330 core
layout(location = 0) in vec3 aPos;

out vec3 TexCoords;

uniform mat4 projection;
uniform mat4 view;

void main()
{
    TexCoords = aPos;
    vec4 pos = projection * view * vec4(aPos, 1.0);
    gl_Position = pos.xyww;
}


#shader fragment
#version 330 core
out vec4 FragColor;

in vec3 TexCoords;// 代表3D纹理坐标的方向向量

uniform samplerCube skybox;// 立方体贴图的纹理采样器

void main()
{
    FragColor = texture(skybox, TexCoords);
}