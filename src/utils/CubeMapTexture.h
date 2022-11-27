#pragma once
#include "Renderer.h"
#include<iostream>
#include<string>
#include <vector>
class CubeMapTexture
{
private:
	unsigned int m_RendererID;
	std::string m_FilePath;
	std::vector<std::string> faces
	{
		"res/textures/skybox/right.jpg",
		"res/textures/skybox/left.jpg",
		"res/textures/skybox/top.jpg",
		"res/textures/skybox/bottom.jpg",
		"res/textures/skybox/front.jpg",
		"res/textures/skybox/back.jpg"
	};

public:
	CubeMapTexture();
	~CubeMapTexture();

	void Bind(unsigned int slot = 0) const;
	void Unbind();
	
};