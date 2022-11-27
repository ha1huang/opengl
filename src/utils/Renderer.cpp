#include "Renderer.h"
#include<iostream>
void GLClearError()
{
	while (glGetError() != GL_NO_ERROR);
}

bool GLLogCall(const char* function, const char* file, int line)
{
	while (GLenum error = glGetError())
	{
		std::cout << "[OpenGL Error] (" << error << ")" << function <<
			" " << file << " : " << line << std::endl;
		return false;
	}
	return true;
}

void Renderer::Clear() const
{
	//清除颜色缓冲和深度缓冲
	GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
}

void Renderer::Draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const
{
	shader.Bind();
	va.Bind();
	ib.Bind();
	//用glDrawElements来替换glDrawArrays函数,使用glDrawElements时，我们会使用当前绑定的索引缓冲对象中的索引进行绘制。
	GLCall(glDrawElements(GL_TRIANGLES, ib.GetCount(), GL_UNSIGNED_INT, nullptr));
}

void Renderer::DrawArrays(const VertexArray& va, const Shader& shader,int first ,int count)const 
{
	shader.Bind();
	va.Bind();
	GLCall(glDrawArrays(GL_TRIANGLES,0,36));
}