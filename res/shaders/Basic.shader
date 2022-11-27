#shader vertex
# version 330 core
// Ϊɶ�õ���vec4,���һ����������͸�ӳ���(Perspective Division)��
/*
* ��ֵ��﷨������(Swizzling)
vec2 someVec;
vec4 differentVec = someVec.xyxx;
vec3 anotherVec = differentVec.zyw;
vec4 otherVec = someVec.xxxx + anotherVec.yxzy;
*/
//������ɫ����ҪΪ���������ṩһ�������layout��ʶ���������ǲ��ܰ������ӵ��������ݡ�
layout(location = 0) in vec4 position;
layout(location = 1) in vec2 texCoord;

//Uniform��һ�ִ�CPU�е�Ӧ����GPU�е���ɫ���������ݵķ�ʽ����uniform�Ͷ���������Щ��ͬ�����ȣ�uniform��ȫ�ֵ�(Global)��
uniform mat4 u_MVP;

//�������뵽��һ��shader��fragment shader��������
out vec2 v_TexCoord;

void main()
{
	//vec4 gl_Position���õ�ֵ���Ϊ�ö�����ɫ�������
	gl_Position = u_MVP * position;
	v_TexCoord = texCoord;
};

#shader fragment
# version 330 core

layout(location = 0) out vec4 color;
//����һ��shader��in ���ƺ����Ͷ�Ҫһ��
in vec2 v_TexCoord;
uniform vec4 u_Color;
uniform sampler2D u_Texture;
void main()
{
	/*������+��ɫ
	uniform vec4 u_Color;
	uniform sampler2D texture1;
	uniform sampler2D texture2;
	void main()
	{
		//mix �����ĵ�����������0.2�᷵��80%�ĵ�һ��������ɫ��20%�ĵڶ���������ɫ����������������Ļ��ɫ��
		FragColor = mix(texture(texture1, TexCoord), texture(texture2, TexCoord), 0.2) * u_Color;
	}
	*/
	vec4 texColor = texture(u_Texture, v_TexCoord);
	color = texColor;
}