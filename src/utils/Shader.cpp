#include "Shader.h"
#include"Renderer.h"
#include <iostream>
#include<fstream>
#include<string>
#include<sstream>
Shader::Shader(const std::string& filepath):m_FilePath(filepath),m_RendererID(0)
{
	ShaderProgramSource source = ParseShader(filepath);
	m_RendererID = CreateShader(source.VertexSource, source.FragmentSource);
}

Shader::~Shader()
{
	GLCall(glDeleteProgram(m_RendererID));
}

void Shader::Bind() const
{
	//
	GLCall(glUseProgram(m_RendererID));
}

void Shader::Unbind() const
{
	GLCall(glUseProgram(0));
}

//����shader����Ӧ��Uniform��ֵ
//��ѯuniform��ַ��Ҫ����֮ǰʹ�ù���ɫ�����򣬵��Ǹ���һ��uniform֮ǰ�������ʹ�ó��򣨵���glUseProgram)


void Shader::SetUniform1i(const std::string& name, int i)
{
	GLCall(glUniform1i(GetUniformLocation(name), i));
}
void Shader::SetUniform1f(const std::string& name, float f0)
{
	GLCall(glUniform1f(GetUniformLocation(name), f0));
}
void Shader::SetUniform1iv(const std::string& name, int count, int* value)
{
	GLCall(glUniform1iv(GetUniformLocation(name), count, value));
}
void Shader::SetUniform3f(const std::string& name, float f0, float f1, float f2)
{
	GLCall(glUniform3f(GetUniformLocation(name), f0, f1, f2));
}
void Shader::SetUniform3fv(const std::string& name, const glm::vec3& value)
{
	GLCall(glUniform3fv(GetUniformLocation(name), 1, &value[0]));
}
void Shader::SetUniform4f(const std::string& name, float f0, float f1, float f2, float f3)
{
	GLCall(glUniform4f(GetUniformLocation(name), f0, f1, f2, f3));
}
void Shader::SetUniform4fv(const std::string& name, const glm::vec4& value)
{
	GLCall(glUniform4fv(GetUniformLocation(name), 1, &value[0]));
}

void Shader::SetUniformMat3f(const std::string& name, const glm::mat3& matrix)
{
	GLCall(glUniformMatrix3fv(GetUniformLocation(name), 1, GL_FALSE, &matrix[0][0]));
}

void Shader::SetUniformMat4f(const std::string& name, const glm::mat4& matrix)
{
	/*
	* �ڶ���������Ҫ���ݶ��ٸ�����
	* ������������дGL_FALSE���������򲼾֣����൱��дGL_TRUE���ǽ�����ת��
	* ���ĸ����������ݵ�ָ�룬Ҳ����ʹ��glm::value_ptr(matrix)
	*/
	GLCall(glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, &matrix[0][0]));
}



int Shader::GetUniformLocation(const std::string& name)
{
	if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end())
		return m_UniformLocationCache[name];
	//��ѯ name ��shader�е�λ��ֵ�����ظ�һ����û�в鵽
	GLCall(int location =  glGetUniformLocation(m_RendererID, name.c_str()));
	if (location==-1)
	{
		std::cout<< "Warning:uniform '" << name << "doesn't exist!"<<std::endl;
	}
	m_UniformLocationCache[name] = location;
	return location;
}

ShaderProgramSource Shader::ParseShader(const std::string& filepath) {
	std::ifstream stream(filepath);
	std::string line;
	std::stringstream ss[2];
	enum class ShaderType {
		NONE = -1, VERTEX = 0, FRAGMENT = 1
	};
	ShaderType type = ShaderType::NONE;
	while (getline(stream, line))
	{
		if (line.find("#shader") != std::string::npos) {
			if (line.find("vertex") != std::string::npos) {
				//set mode ot vertex
				type = ShaderType::VERTEX;
			}
			else if (line.find("fragment") != std::string::npos) {
				type = ShaderType::FRAGMENT;
			}
		}
		else
		{
			ss[(int)type] << line << '\n';
		}
	}
	return { ss[0].str(),ss[1].str() };
}


unsigned int Shader::CompileShader(unsigned int type, const std::string& source) {
	// ����һ��shader,type : ָ���� fragment shader ���� vertex shader
	unsigned int id = glCreateShader(type);
	const char* src = source.c_str();
	//����ɫ��Դ�뽻����ɫ�����󣬵��ĸ�������Ϊnull
	glShaderSource(id, 1, &src, nullptr);
	//����ɫ�����б���
	glCompileShader(id);
	// �鿴�Ƿ����ɹ�
	int result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE) {
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
		char* message = (char*)alloca(length * sizeof(char));
		//��ȡ��ɫ��Դ��Ĵ�����Ϣ
		glGetShaderInfoLog(id, length, &length, message);
		std::cout << "Failed to compile!" << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << std::endl;
		std::cout << message << std::endl;
		glDeleteShader(id);
		return 0;
	}
	return id;
}

unsigned int Shader::CreateShader(const std::string& vertexShader, const std::string& fragmentShader) {
	//�������ɫ����������(Link)Ϊһ����ɫ���������Ȼ������Ⱦ�����ʱ�򼤻������ɫ������
	unsigned int program = glCreateProgram();
	unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
	unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);
	//��������ɫ����һ�������ʱ�������ÿ����ɫ����������ӵ��¸���ɫ�������롣
	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);
	glValidateProgram(program);

	glDeleteShader(vs);
	glDeleteShader(fs);

	return program; 
}
