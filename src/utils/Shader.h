#pragma once
#include<string>
#include<unordered_map>
#include "glm/glm.hpp"
struct ShaderProgramSource
{
	std::string VertexSource;
	std::string FragmentSource;
};
class Shader {
private:
	unsigned int m_RendererID;
	std::string m_FilePath;
	std::unordered_map<std::string, unsigned int> m_UniformLocationCache;

	int GetUniformLocation(const std::string& name);
	unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader);
	unsigned int CompileShader(unsigned int type, const std::string& source);
	ShaderProgramSource ParseShader(const std::string& filepath);
public:
	Shader(const std::string& filepath);
	~Shader();
	void Bind()const;
	void Unbind()const;
	void SetUniform1iv(const std::string& name, int count, int* value);
	void SetUniform1i(const std::string& name, int i);
	void SetUniform1f(const std::string& name, float f0);
	void SetUniform3f(const std::string& name, float f0, float f1, float f2);
	void SetUniform3fv(const std::string& name, const glm::vec3& value);
	void SetUniform4f(const std::string & name, float f0, float f1, float f2, float f3);
	void SetUniform4fv(const std::string& name, const glm::vec4& value);
	void SetUniformMat3f(const std::string& name, const glm::mat3& matrix);
	void SetUniformMat4f(const std::string & name, const glm::mat4& matrix);
};


