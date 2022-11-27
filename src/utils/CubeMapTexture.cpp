#include "CubeMapTexture.h"
#include <stb_image.h>
CubeMapTexture::CubeMapTexture()
	:m_RendererID(0)
{
    glGenTextures(1, &m_RendererID);
    Bind();

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    
    Unbind();
}
CubeMapTexture::~CubeMapTexture()
{
    GLCall(glDeleteTextures(1, &m_RendererID));
}
void CubeMapTexture::CubeMapTexture::Bind(unsigned int slot) const
{
    //GL_TEXTURE0是第一个插槽，以此类推；这个方法是为了激活某个插槽的纹理。
    GLCall(glActiveTexture(GL_TEXTURE0 + slot));
    GLCall(glBindTexture(GL_TEXTURE_2D, m_RendererID));
}
void CubeMapTexture::Unbind()
{
    GLCall(glBindTexture(GL_TEXTURE_2D, 0));
}
