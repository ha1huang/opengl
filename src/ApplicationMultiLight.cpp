#include<iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Renderer.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "VertexArray.h"
#include "IndexBuffer.h"
#include "Shader.h"
#include"MyTexture.h"
#include "camera.h"



void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);


// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

//camare
Camera* camera = Camera::getCamera();
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;
int main(void)
{
	GLFWwindow* window;

	/* Initialize the library */
	if (!glfwInit())
		return -1;


	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	//设置我们的opengl配置文件是核心配置文件
	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Hello World", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);//窗口变化的回调函数
	glfwSetCursorPosCallback(window, mouse_callback);//当mouse移动就就回调这个函数
	glfwSetScrollCallback(window, scroll_callback);//鼠标滚轮的回调函数
	// 控制窗口的刷新频率
	glfwSwapInterval(1);

	// tell GLFW to capture our mouse 先不用哈
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	//glewInit必须在Make the window's context current之后才会成功
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;

	}
	// tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
	stbi_set_flip_vertically_on_load(false);
	{
		//开启深度测试——每次渲染要在Renderer.clear下清除缓冲
		GLCall(glEnable(GL_DEPTH_TEST));

		//GLCall(glEnable(GL_BLEND));
		//GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
		//GLCall(glBlendFunc(GL_ONE, GL_ZERO));

		//1. 初始化数据
		float vertices[] = {
			// positions          // normals           // texture coords
			-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
			 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
			 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
			 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
			-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
			-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

			-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
			 0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
			 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
			 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
			-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
			-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

			-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
			-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
			-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
			-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
			-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
			-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

			 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
			 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
			 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
			 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
			 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
			 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

			-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
			 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
			 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
			 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
			-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
			-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

			-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
			 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
			 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
			 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
			-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
			-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
		};
		// positions all containers
		glm::vec3 cubePositions[] = {
			glm::vec3(0.0f,  0.0f,  0.0f),
			glm::vec3(2.0f,  5.0f, -15.0f),
			glm::vec3(-1.5f, -2.2f, -2.5f),
			glm::vec3(-3.8f, -2.0f, -12.3f),
			glm::vec3(2.4f, -0.4f, -3.5f),
			glm::vec3(-1.7f,  3.0f, -7.5f),
			glm::vec3(1.3f, -2.0f, -2.5f),
			glm::vec3(1.5f,  2.0f, -2.5f),
			glm::vec3(1.5f,  0.2f, -1.5f),
			glm::vec3(-1.3f,  1.0f, -1.5f)
		};
		// positions of the point lights
		glm::vec3 pointLightPositions[] = {
			glm::vec3(0.7f,  0.2f,  2.0f),
			glm::vec3(2.3f, -3.3f, -4.0f),
			glm::vec3(-4.0f,  2.0f, -12.0f),
			glm::vec3(0.0f,  0.0f, -3.0f)
		};
		//2.设置vao、vbo
		VertexBufferLayout layout;
		VertexArray vao,spotVao;
		VertexBuffer vbo(vertices, 6 * 6 * 8 * sizeof(float));
		layout.Push<float>(3);
		layout.Push<float>(3);
		layout.Push<float>(2);
		vao.AddBuffer(vbo, layout);
		spotVao.AddBuffer(vbo, layout);
		//3. 设置shader
		Shader shader("res/shaders/3DSmileLight.shader");
		Shader lightCubeShader("res/shaders/LightCube.shader");

		//4. 设置texture
		MyTexture texture0("res/textures/container2.png");
		MyTexture texture1("res/textures/container2_specular.png");

		shader.Bind();
		shader.SetUniform1i("material.diffuse", 0);
		shader.SetUniform1i("material.specular", 1);

		Renderer renderer;
		Camera* c = Camera::getCamera();
		/* Loop until the user closes the window */
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
			glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
			renderer.Clear();

			//每次渲染都设置纹理
			texture0.Bind(0);
			texture1.Bind(1);


			shader.Bind();
			shader.SetUniform3fv("viewPos", camera->Position);
			shader.SetUniform1f("material.shininess", 32.0f);
			//设置直射光
			shader.SetUniform3f("dirLight.direction", -0.2f, -1.0f, -0.3f);
			shader.SetUniform3f("dirLight.ambient", 0.05f, 0.05f, 0.05f);
			shader.SetUniform3f("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
			shader.SetUniform3f("dirLight.specular", 0.5f, 0.5f, 0.5f);
			//设置点光源
			shader.SetUniform3fv("pointLights[0].position", pointLightPositions[0]);
			shader.SetUniform3f("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
			shader.SetUniform3f("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
			shader.SetUniform3f("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
			shader.SetUniform1f("pointLights[0].constant", 1.0f);
			shader.SetUniform1f("pointLights[0].linear", 0.09f);
			shader.SetUniform1f("pointLights[0].quadratic", 0.032f);

			shader.SetUniform3fv("pointLights[1].position", pointLightPositions[1]);
			shader.SetUniform3f("pointLights[1].ambient", 0.05f, 0.05f, 0.05f);
			shader.SetUniform3f("pointLights[1].diffuse", 0.8f, 0.8f, 0.8f);
			shader.SetUniform3f("pointLights[1].specular", 1.0f, 1.0f, 1.0f);
			shader.SetUniform1f("pointLights[1].constant", 1.0f);
			shader.SetUniform1f("pointLights[1].linear", 0.09f);
			shader.SetUniform1f("pointLights[1].quadratic", 0.032f);

			shader.SetUniform3fv("pointLights[2].position", pointLightPositions[2]);
			shader.SetUniform3f("pointLights[2].ambient", 0.05f, 0.05f, 0.05f);
			shader.SetUniform3f("pointLights[2].diffuse", 0.8f, 0.8f, 0.8f);
			shader.SetUniform3f("pointLights[2].specular", 1.0f, 1.0f, 1.0f);
			shader.SetUniform1f("pointLights[2].constant", 1.0f);
			shader.SetUniform1f("pointLights[2].linear", 0.09f);
			shader.SetUniform1f("pointLights[2].quadratic", 0.032f);

			shader.SetUniform3fv("pointLights[3].position", pointLightPositions[3]);
			shader.SetUniform3f("pointLights[3].ambient", 0.05f, 0.05f, 0.05f);
			shader.SetUniform3f("pointLights[3].diffuse", 0.8f, 0.8f, 0.8f);
			shader.SetUniform3f("pointLights[3].specular", 1.0f, 1.0f, 1.0f);
			shader.SetUniform1f("pointLights[3].constant", 1.0f);
			shader.SetUniform1f("pointLights[3].linear", 0.09f);
			shader.SetUniform1f("pointLights[3].quadratic", 0.032f);

			//设置聚光源（手电筒）
			shader.SetUniform3fv("spotLight.position", camera->Position);
			shader.SetUniform3fv("spotLight.direction", camera->Front);
			shader.SetUniform3f("spotLight.ambient", 0.0f, 0.0f, 0.0f);
			shader.SetUniform3f("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
			shader.SetUniform3f("spotLight.specular", 1.0f, 1.0f, 1.0f);
			shader.SetUniform1f("spotLight.constant", 1.0f);
			shader.SetUniform1f("spotLight.linear", 0.09f);
			shader.SetUniform1f("spotLight.quadratic", 0.032f);
			shader.SetUniform1f("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
			shader.SetUniform1f("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));

			glm::mat4 projection = glm::perspective(glm::radians(camera->Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
			glm::mat4 view = camera->GetViewMatrix();
			shader.Bind();
			shader.SetUniformMat4f("projection", projection);
			shader.SetUniformMat4f("view", view);




			for (int i = 0; i < 10; i++)
			{
				glm::mat4 model = glm::translate(glm::mat4(1.0f), cubePositions[i]);
				float angle = 20.0f * i;
				model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
				shader.SetUniformMat4f("model", model);
				renderer.DrawArrays(vao, shader, 0, 36);
			}

			lightCubeShader.Bind();
			lightCubeShader.SetUniformMat4f("projection", projection);
			lightCubeShader.SetUniformMat4f("view", view);

			for (unsigned int i = 0; i < 4; i++)
			{
				glm::mat4 model = glm::translate(glm::mat4(1.0f), pointLightPositions[i]);
				model = glm::scale(model, glm::vec3(0.2f)); // Make it a smaller cube
				lightCubeShader.SetUniformMat4f("model", model);
				renderer.DrawArrays(spotVao, lightCubeShader, 0, 36);
			}

			/* Swap front and back b uffers */
			glfwSwapBuffers(window);
			/* Poll for and process events */
			glfwPollEvents();
		}
	}
	glfwTerminate();
	return 0;
}
// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
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