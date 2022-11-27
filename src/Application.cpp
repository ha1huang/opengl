#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>

#include <iostream>
#include<vector>
#include<string>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <Shader.h>
#include <camera.h>
#include <model.h>
#include"SkyBox.h"
#include"Texture.h"
#include"terrain.h"
#include"DepthMapFBO.h"
#include"MyTexture.h"
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
unsigned int loadTexture(const char* path);
void drawTrees(Model& tree1, Shader& ourShader, glm::mat4& projection, glm::mat4& view);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera* camera = Camera::getCamera();
float lastX = (float)SCR_WIDTH / 2.0;
float lastY = (float)SCR_HEIGHT / 2.0;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

float tree_x, tree_y, tree_z = 0.0f;
float light_x, light_y, light_z = 0.0f;

int main()
{
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// glfw window creation
	// --------------------
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
	//stbi_set_flip_vertically_on_load(false);

	// configure global opengl state
	// -----------------------------
	glEnable(GL_DEPTH_TEST);

	//main全局变量
	glm::vec3 lightPos(-0.6, 1.20, -1.0);

	float objheigth = 1.0f;

	//阴影初始化
	//设置分辨率
	const unsigned int SHADOW_WIDTH = 5120, SHADOW_HEIGHT = 5120;
	//x,z 左下角   y,w右上角
	glm::vec4 view_size = glm::vec4(-100.0f, 100.0f, -100.0f, 100.0f);
	DepthMapFBO depthMapFBO(SHADOW_WIDTH, SHADOW_HEIGHT, view_size);

	//天空盒初始化 shader 和  texture写死了
	SkyBox skybox;

	//地形初始化
	Shader TerrainShader("res/shaders/TerrainShader.shader");
	string heightMapPath = "res/textures/terrain/heightmap.png";
	std::vector<std::string> ground_path = {
	"res/textures/terrain/blendermap.jpg",
	"res/textures/terrain/background1.jpg",
	"res/textures/terrain/groundR.jpg",
	"res/textures/terrain/groundG.png" ,
	"res/textures/terrain/groundB.jpg"
	};
	TextureManager texman(GL_MIRRORED_REPEAT, GL_MIRRORED_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
	vector<string> types;
	for (int k = 0; k < ground_path.size(); k++)
	{
		types.push_back("texture_diffuse");
	}
	vector<Texture> ground_textures = texman.LoadTexture(ground_path, types);
	Terrain ground(0, 0, ground_textures, heightMapPath, 256.0);


	// build and compile shaders
	// -------------------------
	Shader shader("res/shaders/cubemaps.shader");

	float vertices[] = {
		// back face
		-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
		 1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
		 1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
		 1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
		-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
		-1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
		// front face
		-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
		 1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
		 1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
		 1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
		-1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
		-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
		// left face
		-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
		-1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
		-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
		-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
		-1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
		-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
		// right face
		 1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
		 1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
		 1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right         
		 1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
		 1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
		 1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left     
		// bottom face
		-1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
		 1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
		 1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
		 1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
		-1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
		-1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
		// top face
		-1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
		 1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
		 1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right     
		 1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
		-1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
		-1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left        
	};
	unsigned int cubeVAO, cubeVBO = 0;
	glGenVertexArrays(1, &cubeVAO);
	glGenBuffers(1, &cubeVBO);
	// fill buffer
	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	// link vertex attributes
	glBindVertexArray(cubeVAO);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	// load textures
	// -------------
	unsigned int woodTexture = loadTexture("res/textures/terrain/groundB.jpg");

	// shader configuration
	// --------------------
	shader.Bind();
	shader.SetUniform1i("diffuseTexture", 0);
	shader.SetUniform1i("shadowMap", 1);

	//
	// build and compile shaders
	// -------------------------
	Shader treeShader("res/shaders/huang/Tree.shader");
	Shader houseShader("res/shaders/huang/House2.shader");
	// load models

	Model tree1("res/objects/huang/tree/Tree.obj");
	Model house("res/objects/huang/house/house.obj");

	// render loop
	while (!glfwWindowShouldClose(window))
	{
		// per-frame time logic
		// --------------------
		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// input
		// -----
		processInput(window);

		// render
		// ------
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//先画盒子，画树，带阴影
		 // 1. render depth of scene to texture (from light's perspective)
		// --------------------------------------------------------------
		glm::mat4 lightSpaceMatrix = depthMapFBO.getLightSpaceMatrix(lightPos * 100.0f);
		depthMapFBO.set();
		glm::mat4 model = glm::mat4(1.0f);

		model = glm::translate(model, glm::vec3(0.0f, -0.65, -0.0f));
		depthMapFBO.drawForDepthMap(lightSpaceMatrix, model);
		glBindVertexArray(cubeVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		//model = glm::translate(glm::mat4(1.0f), glm::vec3(4.9f, -1.3f, -45.5f));
		//depthMapFBO.simpleDepthShader.SetUniformMat4f("model", model);
		//tree1.Draw(depthMapFBO.simpleDepthShader);
		glm::mat4 projection = glm::perspective(glm::radians(camera->Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		glm::mat4 view = camera->GetViewMatrix();
		drawTrees(tree1, depthMapFBO.simpleDepthShader, projection, view);
		model = glm::translate(glm::mat4(1.0f), glm::vec3(-0.76, -0.50, -58.30));
		depthMapFBO.simpleDepthShader.SetUniformMat4f("model", model);
		house.Draw(depthMapFBO.simpleDepthShader);

		// reset viewport
		depthMapFBO.reset(SCR_WIDTH, SCR_HEIGHT);
		// 2. render scene as normal using the generated depth/shadow map  
		// --------------------------------------------------------------
		shader.Bind();

		shader.SetUniformMat4f("projection", projection);
		shader.SetUniformMat4f("view", view);
		// set light uniforms
		shader.SetUniform3fv("viewPos", camera->Position);
		shader.SetUniform3fv("lightPos", lightPos);
		shader.SetUniformMat4f("lightSpaceMatrix", lightSpaceMatrix);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, woodTexture);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, depthMapFBO.depthMap);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, -0.65, 0.0f));
		shader.SetUniformMat4f("model", model);
		glBindVertexArray(cubeVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);


		//画天空盒
		glm::mat4 skyboxview = glm::mat4(glm::mat3(camera->GetViewMatrix()));
		skybox.drawSkyBox(skyboxview, projection);


		////画地形
		TerrainShader.Bind();
		glActiveTexture(GL_TEXTURE5);//因为0-4已经绑定了terrain的纹理
		glBindTexture(GL_TEXTURE_2D, depthMapFBO.depthMap);
		TerrainShader.SetUniform1i("shadowMap", 5);//设置阴影贴图
		//TerrainShader.SetUniform4fv("plane", clipping_plane);//切掉一些
		TerrainShader.SetUniformMat4f("lightSpaceMatrix", lightSpaceMatrix);
		TerrainShader.SetUniform3f("dirLight.direction", 2.0f, -4.0f, 1.0f);
		TerrainShader.SetUniform3f("dirLight.ambient", 0.05f, 0.05f, 0.05f);
		TerrainShader.SetUniform3f("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
		TerrainShader.SetUniform3f("dirLight.specular", 0.5f, 0.5f, 0.5f);
		TerrainShader.SetUniformMat4f("projection", projection);
		TerrainShader.SetUniformMat4f("view", view);
		//TerrainShader.SetUniform4fv("plane", clippling_plane);
		TerrainShader.SetUniform3fv("viewPos", camera->Position);
		TerrainShader.SetUniform3fv("skyColor", glm::vec3(0.527f, 0.805f, 0.918f));
		ground.Draw(TerrainShader);

		//画树
		treeShader.Bind();
		treeShader.SetUniform3f("dirLight.direction", 2.0f, -4.0f, 1.0f);
		treeShader.SetUniform3f("dirLight.ambient", 0.05f, 0.05f, 0.05f);
		treeShader.SetUniform3f("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
		treeShader.SetUniform3f("dirLight.specular", 0.5f, 0.5f, 0.5f);
		treeShader.SetUniform3fv("viewPos", camera->Position);
		drawTrees(tree1, treeShader, projection, view);

		//画房子
		houseShader.Bind();
		model = glm::translate(glm::mat4(1.0f), glm::vec3(-0.76, -0.60, -58.30));
		houseShader.SetUniformMat4f("projection", projection);
		houseShader.SetUniformMat4f("view", view);
		//houseShader.SetUniform3f("dirLight.direction", 2.0f, -4.0f, 1.0f);
		houseShader.SetUniform3fv("lightPos", lightPos * 100.0f);
		houseShader.SetUniform3fv("viewPos", camera->Position);
		houseShader.SetUniformMat4f("model", model);
		house.Draw(houseShader);

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// optional: de-allocate all resources once they've outlived their purpose:
	// ------------------------------------------------------------------------


	glfwTerminate();
	return 0;
}
//画树
void drawTrees(Model& tree1, Shader& ourShader, glm::mat4& projection, glm::mat4& view) {

	// view/projection transformations
	ourShader.SetUniformMat4f("projection", projection);
	ourShader.SetUniformMat4f("view", view);


	glm::mat4 model = glm::mat4(1.0f);
	// render the loaded model
	//model = glm::translate(model, glm::vec3(tree_x, tree_y, tree_z)); // translate it down so it's at the center of the scene
	model = glm::translate(model, glm::vec3(4.9f, -1.3f, -45.5f)); // translate it down so it's at the center of the scene
	model = glm::rotate(model, glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	ourShader.SetUniformMat4f("model", model);
	tree1.Draw(ourShader);
	model = glm::translate(glm::mat4(1.0f), glm::vec3(9.3f, -1.3f, -53.0f));
	model = glm::rotate(model, glm::radians(30.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	ourShader.SetUniformMat4f("model", model);
	tree1.Draw(ourShader);
	model = glm::translate(glm::mat4(1.0f), glm::vec3(10.0f, -0.5f, -61.0f));
	model = glm::rotate(model, glm::radians(20.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	ourShader.SetUniformMat4f("model", model);
	tree1.Draw(ourShader);
	model = glm::translate(glm::mat4(1.0f), glm::vec3(-1.8f, -0.7f, -47.0f));
	ourShader.SetUniformMat4f("model", model);
	tree1.Draw(ourShader);
}


// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS)
		tree_x += 0.02;
	if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
		tree_x -= 0.02;
	if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS)
		tree_y += 0.02;
	if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
		tree_y -= 0.02;
	if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS)
		tree_z += 0.02;
	if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
		tree_z -= 0.02;
	if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS)
		cout << "(" << tree_x << "," << tree_y << "," << tree_z << ")" << endl;

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera->ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera->ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera->ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera->ProcessKeyboard(RIGHT, deltaTime);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
	float xpos = static_cast<float>(xposIn);
	float ypos = static_cast<float>(yposIn);
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera->ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera->ProcessMouseScroll(static_cast<float>(yoffset));
}

// utility function for loading a 2D texture from file
// ---------------------------------------------------
unsigned int loadTexture(char const* path)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}


