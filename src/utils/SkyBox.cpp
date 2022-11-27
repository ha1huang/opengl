#include"SkyBox.h"
#include"VertexArray.h"
#include"VertexBuffer.h"

SkyBox::SkyBox()
{	
	//…Ë÷√vertex
	float skyboxVertices[] = {
		// positions          
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f
	};
	unsigned int skyboxVAO, skyboxVBO;
	VertexBuffer vbo(skyboxVertices, 3 * 6 * 6 * sizeof(float));
	VertexBufferLayout layout;
	layout.Push<float>(3);
	vao.AddBuffer(vbo, layout);
	vao.Unbind();
	//…Ë÷√shader
	skyboxShader.Bind();
	skyboxShader.SetUniform1i("skybox", 0);

}

void SkyBox::drawSkyBox(glm::mat4 view, glm::mat4 projection)
{
	//skybox.draw(view,projection)
// draw skybox as last
	glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
	skyboxShader.Bind();
	skyboxShader.SetUniformMat4f("view", view);
	skyboxShader.SetUniformMat4f("projection", projection);
	// skybox cube
	vao.Bind();
	glActiveTexture(GL_TEXTURE0);
	cubeMapTextrue.Bind();
	glDrawArrays(GL_TRIANGLES, 0, 36);
	vao.Unbind();
	glDepthFunc(GL_LESS); // set depth function back to default
}
