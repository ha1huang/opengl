#include "VertexBuffer.h"
#include "Renderer.h"
VertexBuffer::VertexBuffer(const void* data, unsigned int size)
{
	GLCall(glGenBuffers(1, &m_RendererID));//生成一个buffer
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_RendererID));//将buffer扔给了GPU
	//creates and initializes a buffer object's data store 			
	/*
	将数据扔给vertex buffer，
	*第一个参数：目标缓冲类型
	*第四个参数：
		GL_STATIC_DRAW ：数据不会或几乎不会改变。
		GL_DYNAMIC_DRAW：数据会被改变很多。
		GL_STREAM_DRAW ：数据每次绘制时都会改变。
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
