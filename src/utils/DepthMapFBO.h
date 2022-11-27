#pragma once
#include <glad/glad.h>
#include"Shader.h"
class DepthMapFBO
{
public:
	unsigned int depthMapFBO;
	unsigned int depthMap;
	unsigned int SHADOW_WIDTH;
	unsigned int SHADOW_HEIGHT;
	Shader simpleDepthShader = Shader("res/shaders/ShadowMappingDepth.shader");
	glm::mat4 lightSpaceMatrix;
	float near_plane = 1.0f, far_plane = 1000.0f;
	//lightProjection = glm::perspective(glm::radians(45.0f), (GLfloat)SHADOW_WIDTH / (GLfloat)SHADOW_HEIGHT, near_plane, far_plane); // note that if you use a perspective projection matrix you'll have to change the light position as the current light position isn't enough to reflect the whole scene
	glm::mat4 lightProjection;
	DepthMapFBO(float shadowWidth, float shadowHeigh, glm::vec4 view_size) {
		SHADOW_WIDTH = shadowWidth;
		SHADOW_HEIGHT = shadowHeigh;
		lightProjection = glm::ortho(view_size.x, view_size.y, view_size.z, view_size.w, near_plane, far_plane);
		init();
	};
	~DepthMapFBO() {};
	glm::mat4 getLightSpaceMatrix(glm::vec3 lightPos) {
		glm::mat4 lightView;
		lightView = glm::lookAt(lightPos, glm::vec3(4.9f, -1.3f, -45.5f), glm::vec3(0.0, 1.0, 0.0));
		lightSpaceMatrix = lightProjection * lightView;
		return lightSpaceMatrix;
	}
	void init() {
		// configure depth map FBO
	// -----------------------
		glGenFramebuffers(1, &depthMapFBO);
		// create depth texture
		glGenTextures(1, &depthMap);
		glBindTexture(GL_TEXTURE_2D, depthMap);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
		// attach depth texture as FBO's depth buffer
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	void Bind() {
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	}
	void UnBind() {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	void set() {
		Bind();
		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glClear(GL_DEPTH_BUFFER_BIT);
	}
	void drawForDepthMap(glm::mat4 lightSpaceMatrix, glm::mat4 model) {
		simpleDepthShader.Bind();
		simpleDepthShader.SetUniformMat4f("lightSpaceMatrix", lightSpaceMatrix);
		simpleDepthShader.SetUniformMat4f("model", model);
	}
	void setModel(glm::mat4 model) {

	}
	void reset(float SCR_WIDTH, float SCR_HEIGHT) {
		// reset viewport
		UnBind();
		glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}
private:

};
