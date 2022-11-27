#include "VertexArray.h"
#include"Renderer.h"
#include"VertexBufferLayout.h"
VertexArray::VertexArray()
{
	GLCall(glGenVertexArrays(1, &m_RendererID));
}

VertexArray::~VertexArray()
{
	GLCall(glDeleteVertexArrays(1, &m_RendererID));
}

void VertexArray::AddBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout)
{
	Bind();
	vb.Bind();
	const auto& elements = layout.GetElements();
	unsigned int offset = 0;
	for (unsigned int i = 0; i < elements.size(); i++)
	{
		const auto& element = elements[i];
		//使用glEnableVertexAttribArray，以顶点属性位置值作为参数，启用顶点属性；顶点属性默认是禁用的。
		GLCall(glEnableVertexAttribArray(i));
		/*
		glVertexAttribPointer函数告诉OpenGL该如何解析顶点数据
		第一个参数：对应shader中的 layout(location = 0)
		第三个参数：表示数据是否被标准化——如果我们设置为GL_TRUE，所有数据都会被映射到0（对于有符号型signed数据是-1）到1之间。
		*/
		GLCall(glVertexAttribPointer(i, element.count, element.type, element.normalized,
			layout.Getstride(), (const void*)offset));
		offset += element.count * VertexBufferElement::GetsizeofType(element.type);
	}
}

void VertexArray::Bind() const
{
	GLCall(glBindVertexArray(m_RendererID));
}

void VertexArray::Unbind() const
{
	GLCall(glBindVertexArray(0));
}
