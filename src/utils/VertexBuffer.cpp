#include "VertexBuffer.h"
#include "Renderer.h"
VertexBuffer::VertexBuffer(const void* data, unsigned int size)
{
	GLCall(glGenBuffers(1, &m_RendererID));//����һ��buffer
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_RendererID));//��buffer�Ӹ���GPU
	//creates and initializes a buffer object's data store 			
	/*
	�������Ӹ�vertex buffer��
	*��һ��������Ŀ�껺������
	*���ĸ�������
		GL_STATIC_DRAW �����ݲ���򼸺�����ı䡣
		GL_DYNAMIC_DRAW�����ݻᱻ�ı�ܶࡣ
		GL_STREAM_DRAW ������ÿ�λ���ʱ����ı䡣
	*/
	GLCall(glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW));
}

VertexBuffer::~VertexBuffer()
{
	GLCall(glDeleteBuffers(1,&m_RendererID));
}

void VertexBuffer::Bind()const
{
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_RendererID));
}

void VertexBuffer::Unbind()const
{
	GLCall(glBindBuffer(GL_ARRAY_BUFFER,0));
}
