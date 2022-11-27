#include "MyTexture.h"
#include <stb_image.h>

MyTexture::MyTexture(const std::string& path)
	:m_RendererID(0), m_FilePath(path), m_LocalBuffer(nullptr), m_Width(0), m_Height(0), m_BPP(0)
{
	//stbi_set_flip_vertically_on_load(1); /* 垂直翻转纹理,因为opengl绘制的时候是从左下角开始绘制的 */
	m_LocalBuffer = stbi_load(path.c_str(), &m_Width, &m_Height, &m_BPP, 4);//为什么是4？因为rgb有四个通道

	GLCall(glGenTextures(1, &m_RendererID));
	//无需glActiveTexture(GL_TEXTURE0 + slot)，因为GL_TEXTURE0默认是激活的
	GLCall(glBindTexture(GL_TEXTURE_2D, m_RendererID));

	/*
	* 纹理过滤
	* 第一个参数：  指定纹理的目标
	* 第二个参数：  指定放大还是缩小，应用相应的纹理过滤方式
	* 第三个参数：  过滤方式
	*			    GL_NEAREST：临近过滤——OpenGL会选择中心点最接近纹理坐标的那个像素。
	*			    GL_LINEAR： 线性过滤——它会基于纹理坐标附近的纹理像素，计算出一个插值，近似出这些纹理像素之间的颜色。输出更真实
	*               问题：缩小的时候，一个纹理坐标对应的像素坐标跨度很大，使得不真实，而且过高的分辨率浪费内存。
	*				手工为每个纹理图像创建一系列多级渐远纹理很麻烦，幸好OpenGL有一个glGenerateMipmaps函数，在创建完一个纹理后调用它OpenGL就会承担接下来的所有工作了。
					GL_NEAREST_MIPMAP_NEAREST 	使用最邻近的多级渐远纹理来匹配像素大小，并使用邻近插值进行纹理采样
					GL_LINEAR_MIPMAP_NEAREST 	使用最邻近的多级渐远纹理级别，并使用线性插值进行采样
					GL_NEAREST_MIPMAP_LINEAR 	在两个最匹配像素大小的多级渐远纹理之间进行线性插值，使用邻近插值进行采样
					GL_LINEAR_MIPMAP_LINEAR 	在两个邻近的多级渐远纹理之间使用线性插值，并使用线性插值进行采样
	*				一个常见的错误是，将放大过滤的选项设置为多级渐远纹理过滤选项之一。
	*/				
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	/*纹理环绕方式
	* s,t,r 等价于 x，y，z
	* 第一个参数：指定纹理的目标
	* 第二个参数：指定要设定的纹理轴
	* 第三个参数：传递一个环绕方式
	*		GL_REPEAT 			对纹理的默认行为。重复纹理图像。
			GL_MIRRORED_REPEAT 	和GL_REPEAT一样，但每次重复图片是镜像放置的。
			GL_CLAMP_TO_EDGE 	纹理坐标会被约束在0到1之间，超出的部分会重复纹理坐标的边缘，产生一种边缘被拉伸的效果。
			GL_CLAMP_TO_BORDER 	超出的坐标为用户指定的边缘颜色。传递一个float数组作为边缘的颜色值。其中fv表示float vector/arry
								float borderColor[] = { 1.0f, 1.0f, 0.0f, 1.0f };
								glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	*/
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
	
	if (m_LocalBuffer) {
		/*绑定2D纹理
		* 第一个参数:指定了纹理目标(Target)。任何绑定到GL_TEXTURE_1D和GL_TEXTURE_3D的纹理不会受到影响
		* 第二个参数:为纹理指定多级渐远纹理的级别，这里我们填0，也就是基本级别。
		* 第三个参数:告诉OpenGL我们希望把纹理储存为何种格式。我们的图像只有RGB值，因此我们也把纹理储存为RGB值。
		* 第六个参数:应该总是被设为0（历史遗留的问题）。
		* 第七第八个参数：定义了源图的格式和数据类型。
		* 最后一个参数是真正的图像数据。
		*/
		GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_LocalBuffer));
		//这会为当前绑定的纹理自动生成所有需要的多级渐远纹理。
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	//取消绑定
	GLCall(glBindTexture(GL_TEXTURE_2D, 0));

	if (m_LocalBuffer) {
		stbi_image_free(m_LocalBuffer);
	}
}

MyTexture::~MyTexture()
{
	GLCall(glDeleteTextures(1, &m_RendererID));
}

void MyTexture::Bind(unsigned int slot) const
{
	//GL_TEXTURE0是第一个插槽，以此类推；这个方法是为了激活某个插槽的纹理。
	GLCall(glActiveTexture(GL_TEXTURE0 + slot));
	GLCall(glBindTexture(GL_TEXTURE_2D, m_RendererID));
}

void MyTexture::Unbind()
{
	GLCall(glBindTexture(GL_TEXTURE_2D, 0));
}
