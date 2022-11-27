#shader vertex
# version 330 core
// 为啥用的是vec4,最后一个参数用在透视除法(Perspective Division)上
/*
* 奇怪的语法：重组(Swizzling)
vec2 someVec;
vec4 differentVec = someVec.xyxx;
vec3 anotherVec = differentVec.zyw;
vec4 otherVec = someVec.xxxx + anotherVec.yxzy;
*/
//顶点着色器需要为它的输入提供一个额外的layout标识，这样我们才能把它链接到顶点数据。
layout(location = 0) in vec4 position;
layout(location = 1) in vec2 texCoord;

//Uniform是一种从CPU中的应用向GPU中的着色器发送数据的方式，但uniform和顶点属性有些不同。首先，uniform是全局的(Global)。
uniform mat4 u_MVP;

//输出会进入到下一个shader（fragment shader）的输入
out vec2 v_TexCoord;

void main()
{
	//vec4 gl_Position设置的值会成为该顶点着色器的输出
	gl_Position = u_MVP * position;
	v_TexCoord = texCoord;
};

#shader fragment
# version 330 core

layout(location = 0) out vec4 color;
//和上一个shader的in 名称和类型都要一致
in vec2 v_TexCoord;
uniform vec4 u_Color;
uniform sampler2D u_Texture;
void main()
{
	/*纹理混合+颜色
	uniform vec4 u_Color;
	uniform sampler2D texture1;
	uniform sampler2D texture2;
	void main()
	{
		//mix 函数的第三个参数：0.2会返回80%的第一个输入颜色和20%的第二个输入颜色，即返回两个纹理的混合色。
		FragColor = mix(texture(texture1, TexCoord), texture(texture2, TexCoord), 0.2) * u_Color;
	}
	*/
	vec4 texColor = texture(u_Texture, v_TexCoord);
	color = texColor;
}