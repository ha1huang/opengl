#pragma once
#include"Shader.h"
#include"VertexBufferLayout.h"
#include"CubeMapTexture.h"
class SkyBox {
private:
	CubeMapTexture cubeMapTextrue;
	
	VertexArray vao;
public:
	Shader skyboxShader = Shader("res/shaders/skybox.shader");
	SkyBox();
	void drawSkyBox(glm::mat4 view, glm::mat4 projection);
};