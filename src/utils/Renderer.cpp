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
	//�����ɫ�������Ȼ���
	GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
}

void Renderer::Draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const
{
	shader.Bind();
	va.Bind();
	ib.Bind();
	//��glDrawElements���滻glDrawArrays����,ʹ��glDrawElementsʱ�����ǻ�ʹ�õ�ǰ�󶨵�������������е��������л��ơ�
	GLCall(glDrawElements(GL_TRIANGLES, ib.GetCount(), GL_UNSIGNED_INT, nullptr));
}

void Renderer::DrawArrays(const VertexArray& va, const Shader& shader,int first ,int count)const 
{
	shader.Bind();
	va.Bind();
	GLCall(glDrawArrays(GL_TRIANGLES,0,36));
}