#include "MyTexture.h"
#include <stb_image.h>

MyTexture::MyTexture(const std::string& path)
	:m_RendererID(0), m_FilePath(path), m_LocalBuffer(nullptr), m_Width(0), m_Height(0), m_BPP(0)
{
	//stbi_set_flip_vertically_on_load(1); /* ��ֱ��ת����,��Ϊopengl���Ƶ�ʱ���Ǵ����½ǿ�ʼ���Ƶ� */
	m_LocalBuffer = stbi_load(path.c_str(), &m_Width, &m_Height, &m_BPP, 4);//Ϊʲô��4����Ϊrgb���ĸ�ͨ��

	GLCall(glGenTextures(1, &m_RendererID));
	//����glActiveTexture(GL_TEXTURE0 + slot)����ΪGL_TEXTURE0Ĭ���Ǽ����
	GLCall(glBindTexture(GL_TEXTURE_2D, m_RendererID));

	/*
	* �������
	* ��һ��������  ָ�������Ŀ��
	* �ڶ���������  ָ���Ŵ�����С��Ӧ����Ӧ��������˷�ʽ
	* ������������  ���˷�ʽ
	*			    GL_NEAREST���ٽ����ˡ���OpenGL��ѡ�����ĵ���ӽ�����������Ǹ����ء�
	*			    GL_LINEAR�� ���Թ��ˡ�����������������긽�����������أ������һ����ֵ�����Ƴ���Щ��������֮�����ɫ���������ʵ
	*               ���⣺��С��ʱ��һ�����������Ӧ�����������Ⱥܴ�ʹ�ò���ʵ�����ҹ��ߵķֱ����˷��ڴ档
	*				�ֹ�Ϊÿ������ͼ�񴴽�һϵ�ж༶��Զ������鷳���Һ�OpenGL��һ��glGenerateMipmaps�������ڴ�����һ������������OpenGL�ͻ�е������������й����ˡ�
					GL_NEAREST_MIPMAP_NEAREST 	ʹ�����ڽ��Ķ༶��Զ������ƥ�����ش�С����ʹ���ڽ���ֵ�����������
					GL_LINEAR_MIPMAP_NEAREST 	ʹ�����ڽ��Ķ༶��Զ�����𣬲�ʹ�����Բ�ֵ���в���
					GL_NEAREST_MIPMAP_LINEAR 	��������ƥ�����ش�С�Ķ༶��Զ����֮��������Բ�ֵ��ʹ���ڽ���ֵ���в���
					GL_LINEAR_MIPMAP_LINEAR 	�������ڽ��Ķ༶��Զ����֮��ʹ�����Բ�ֵ����ʹ�����Բ�ֵ���в���
	*				һ�������Ĵ����ǣ����Ŵ���˵�ѡ������Ϊ�༶��Զ�������ѡ��֮һ��
	*/				
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	/*�����Ʒ�ʽ
	* s,t,r �ȼ��� x��y��z
	* ��һ��������ָ�������Ŀ��
	* �ڶ���������ָ��Ҫ�趨��������
	* ����������������һ�����Ʒ�ʽ
	*		GL_REPEAT 			�������Ĭ����Ϊ���ظ�����ͼ��
			GL_MIRRORED_REPEAT 	��GL_REPEATһ������ÿ���ظ�ͼƬ�Ǿ�����õġ�
			GL_CLAMP_TO_EDGE 	��������ᱻԼ����0��1֮�䣬�����Ĳ��ֻ��ظ���������ı�Ե������һ�ֱ�Ե�������Ч����
			GL_CLAMP_TO_BORDER 	����������Ϊ�û�ָ���ı�Ե��ɫ������һ��float������Ϊ��Ե����ɫֵ������fv��ʾfloat vector/arry
								float borderColor[] = { 1.0f, 1.0f, 0.0f, 1.0f };
								glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	*/
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
	
	if (m_LocalBuffer) {
		/*��2D����
		* ��һ������:ָ��������Ŀ��(Target)���κΰ󶨵�GL_TEXTURE_1D��GL_TEXTURE_3D���������ܵ�Ӱ��
		* �ڶ�������:Ϊ����ָ���༶��Զ����ļ�������������0��Ҳ���ǻ�������
		* ����������:����OpenGL����ϣ����������Ϊ���ָ�ʽ�����ǵ�ͼ��ֻ��RGBֵ���������Ҳ��������ΪRGBֵ��
		* ����������:Ӧ�����Ǳ���Ϊ0����ʷ���������⣩��
		* ���ߵڰ˸�������������Դͼ�ĸ�ʽ���������͡�
		* ���һ��������������ͼ�����ݡ�
		*/
		GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_LocalBuffer));
		//���Ϊ��ǰ�󶨵������Զ�����������Ҫ�Ķ༶��Զ����
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	//ȡ����
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
	//GL_TEXTURE0�ǵ�һ����ۣ��Դ����ƣ����������Ϊ�˼���ĳ����۵�����
	GLCall(glActiveTexture(GL_TEXTURE0 + slot));
	GLCall(glBindTexture(GL_TEXTURE_2D, m_RendererID));
}

void MyTexture::Unbind()
{
	GLCall(glBindTexture(GL_TEXTURE_2D, 0));
}
