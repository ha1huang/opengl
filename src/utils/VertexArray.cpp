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
		//ʹ��glEnableVertexAttribArray���Զ�������λ��ֵ��Ϊ���������ö������ԣ���������Ĭ���ǽ��õġ�
		GLCall(glEnableVertexAttribArray(i));
		/*
		glVertexAttribPointer��������OpenGL����ν�����������
		��һ����������Ӧshader�е� layout(location = 0)
		��������������ʾ�����Ƿ񱻱�׼�����������������ΪGL_TRUE���������ݶ��ᱻӳ�䵽0�������з�����signed������-1����1֮�䡣
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
