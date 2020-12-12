#include <iostream>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <SOIL/SOIL.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <Shaders/shader.h>
#include <camera.h>
#include "stb_image.h"


// Window dimensions
const GLuint WIDTH = 1400, HEIGHT = 800;


glm::vec3 cameraPos(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp(0.0f, 1.0f, 0.0f);
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = WIDTH / 2.0f;
float lastY = HEIGHT / 2.0f;
bool firstMouse = true;

glm::vec3 sunLightDir(-10.0f, -10.0f, 10.0f);
glm::vec3 pointLightsPos[] = {
	glm::vec3(-3.0f, -1.0f, -2.0f),
	glm::vec3(2.0f, 1.0f, -3.0f),
	glm::vec3(2.0f, 0.0f, 2.0f)
};
glm::vec3 lightColorWhite(1.0f, 1.0f, 1.0f);
glm::vec3 lightColorRed(1.0f, 0.0f, 0.0f);
glm::vec3 lightColorGreen(0.0f, 1.0f, 0.0f);
glm::vec3 lightColorBlue(0.0f, 0.0f, 1.0f);
glm::vec3 borderColor(0.78f, 0.1f, 0.52f);

struct Material {
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	float shininess;
};

void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
unsigned int loadTexture(const char *path);


int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	// Window
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Road to Zachet", nullptr, nullptr);
	if (window == nullptr) {
		std::cout << "Window creation error" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) {
		std::cout << "GLEW init error" << std::endl;
		return -1;
	}

	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	

	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);

	// Shaders
	Shader objectShader("../LibStuff/Include/Shaders/src/vshader3.vsh", "../LibStuff/Include/Shaders/src/fshader3.fsh");
	Shader lightShader("../LibStuff/Include/Shaders/src/vlight1.vsh", "../LibStuff/Include/Shaders/src/flight1.fsh");
	Shader borderShader("../LibStuff/Include/Shaders/src/vshader3.vsh", "../LibStuff/Include/Shaders/src/fborder.fsh");

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_STENCIL_TEST);
	glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

	// Vertices
	GLfloat vertTriangle[] = {
		-0.5f, -0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   0.0f, 0.0f,  0.0f, 0.0f, 1.0f,
		0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   4.0f, 0.0f,  0.0f, 0.0f, 1.0f,
		0.0f,  0.5f, 0.0f,   0.3f, 0.3f, 0.3f,   2.0f, 4.0f,  0.0f, 0.0f, 1.0f,

		0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   0.0f, 0.0f,   0.66f, 0.34f, -0.66f, 
		0.0f, -0.5f, -0.5f,   0.0f, 0.0f, 1.0f,   4.0f, 0.0f,  0.66f, 0.34f, -0.66f,
		0.0f,  0.5f, 0.0f,   0.3f, 0.3f, 0.3f,   2.0f, 4.0f,  0.66f, 0.34f, -0.66f,

		0.0f, -0.5f, -0.5f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,  -0.66f, 0.34f, -0.66f,
		-0.5f, -0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   4.0f, 0.0f,  -0.66f, 0.34f, -0.66f,
		0.0f,  0.5f, 0.0f,   0.3f, 0.3f, 0.3f,   2.0f, 4.0f,  -0.66f, 0.34f, -0.66f,

		-0.5f, -0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   0.0f, 0.0f,  0.0f, 1.0f, 0.0f,
		0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   4.0f, 0.0f,  0.0f, 1.0f, 0.0f,
		0.0f, -0.5f, -0.5f,   0.0f, 0.0f, 1.0f,   2.0f, 4.0f,  0.0f, 1.0f, 0.0f
	};

	GLuint VBO[2], VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(2, VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertTriangle), vertTriangle, GL_STATIC_DRAW);
	
	glBindVertexArray(VAO);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(8 * sizeof(GLfloat)));
	glEnableVertexAttribArray(3);
	
	glBindVertexArray(0);


	GLfloat vertCube[] = {
		-0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f,
		 0.5f,  0.5f, -0.5f,
		 0.5f,  0.5f, -0.5f,
		-0.5f,  0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,

		-0.5f, -0.5f,  0.5f,
		 0.5f, -0.5f,  0.5f,
		 0.5f,  0.5f,  0.5f,
		 0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,
		-0.5f, -0.5f,  0.5f,

		-0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,
		-0.5f, -0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,

		 0.5f,  0.5f,  0.5f,
		 0.5f,  0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f,  0.5f,
		 0.5f,  0.5f,  0.5f,

		-0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f,  0.5f,
		 0.5f, -0.5f,  0.5f,
		-0.5f, -0.5f,  0.5f,
		-0.5f, -0.5f, -0.5f,

		-0.5f,  0.5f, -0.5f,
		 0.5f,  0.5f, -0.5f,
		 0.5f,  0.5f,  0.5f,
		 0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f, -0.5f,
	};

	GLuint lightVAO;

	glGenVertexArrays(1, &lightVAO);
	glGenBuffers(1, &VBO[1]);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertCube), vertCube, GL_STATIC_DRAW);

	glBindVertexArray(lightVAO);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);


	GLfloat vertPlane[] = {
		 10.0f, -3.0f,  10.0f,  2.0f, 0.0f,  0.0f, 1.0f, 0.0f,
		-10.0f, -3.0f,  10.0f,  0.0f, 0.0f,  0.0f, 1.0f, 0.0f,
		-10.0f, -3.0f, -10.0f,  0.0f, 2.0f,  0.0f, 1.0f, 0.0f,

		 10.0f, -3.0f,  10.0f,  2.0f, 0.0f,  0.0f, 1.0f, 0.0f,
		-10.0f, -3.0f, -10.0f,  0.0f, 2.0f,  0.0f, 1.0f, 0.0f,
		 10.0f, -3.0f, -10.0f,  2.0f, 2.0f,  0.0f, 1.0f, 0.0f
	};

	GLuint planeVAO, planeVBO;
	glGenVertexArrays(1, &planeVAO);
	glGenBuffers(1, &planeVBO);
	
	glBindBuffer(GL_ARRAY_BUFFER, planeVBO); 
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertPlane), &vertPlane, GL_STATIC_DRAW); 
	
	glBindVertexArray(planeVAO);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)0);
	
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));

	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(5 * sizeof(GLfloat)));
	
	glBindVertexArray(0);


	GLuint diffuseMap = loadTexture("container_diff.png");
	GLuint specularMap = loadTexture("container_spec.png");
	GLuint emissionMap = loadTexture("container_emission.jpg");
	GLuint floorMap = loadTexture("floor_laminate.jpg");


	glm::vec3 uniquePositions[] = {
        glm::vec3(0.0f,  0.0f,  0.0f),
        glm::vec3(2.0f,  5.0f, -9.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -9.3f),
        glm::vec3(2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f,  3.0f, -7.5f),
        glm::vec3(1.3f, -2.0f, -2.5f),
        glm::vec3(1.5f,  2.0f, -2.5f),
        glm::vec3(1.5f,  0.2f, -1.5f),
        glm::vec3(-1.3f,  1.0f, -1.5f)
	};


	Material gold{ glm::vec3(0.24725f, 0.1995f, 0.0745),
				   glm::vec3(0.75164f, 0.60648f, 0.22648f),
				   glm::vec3(0.628281f, 0.555802f, 0.366065f),
				   51.2f };

	Material chrome{ glm::vec3(0.25f, 0.25f, 0.25f),
				     glm::vec3(0.4f, 0.4f, 0.4f),
				     glm::vec3(0.774597f, 0.774597f, 0.774597f),
				     76.8f
	};


	// Run
	// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	// glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	while (!glfwWindowShouldClose(window)) {
		GLfloat curTime = (GLfloat)glfwGetTime();
		deltaTime = curTime - lastTime;
		lastTime = curTime;

		processInput(window);
		// Rendering
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		
		objectShader.Use();
		
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, diffuseMap);
		glUniform1i(glGetUniformLocation(objectShader.Program, "material.diffuse"), 0);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, specularMap);
		glUniform1i(glGetUniformLocation(objectShader.Program, "material.specular"), 1);

		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, emissionMap);
		glUniform1i(glGetUniformLocation(objectShader.Program, "material.emission"), 2);


		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 100.0f);

		glUniformMatrix4fv(glGetUniformLocation(objectShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(objectShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));


		//glUniform3fv(glGetUniformLocation(objectShader.Program, "lightColor"), 1, &lightColorWhite[0]);
		//glUniform3fv(glGetUniformLocation(objectShader.Program, "lightPos"), 1, &sunLightPos[0]);
		glUniform3fv(glGetUniformLocation(objectShader.Program, "viewPos"), 1, &camera.Position[0]);

		/*
		glUniform3fv(glGetUniformLocation(objectShader.Program, "material.ambient"), 1, &gold.ambient[0]);
		glUniform3fv(glGetUniformLocation(objectShader.Program, "material.diffuse"), 1, &gold.diffuse[0]);
		glUniform3fv(glGetUniformLocation(objectShader.Program, "material.specular"), 1, &gold.specular[0]);
		glUniform1f(glGetUniformLocation(objectShader.Program, "material.shininess"), gold.shininess);
		*/
		glUniform1f(glGetUniformLocation(objectShader.Program, "material.shininess"), chrome.shininess);

		glUniform1f(glGetUniformLocation(objectShader.Program, "curTime"), curTime);

		glm::vec3 lightColor(1.0f);
		glm::vec3 diffuseColor = lightColor * glm::vec3(0.5f);
		glm::vec3 ambientColor = diffuseColor * glm::vec3(0.2f);
		glm::vec3 specularColor = 0.5f * lightColor;
		glUniform3fv(glGetUniformLocation(objectShader.Program, "dirLight.direction"), 1, &sunLightDir[0]);
		glUniform3fv(glGetUniformLocation(objectShader.Program, "dirLight.ambient"), 1, &ambientColor[0]);
		glUniform3fv(glGetUniformLocation(objectShader.Program, "dirLight.diffuse"), 1, &diffuseColor[0]);
		glUniform3fv(glGetUniformLocation(objectShader.Program, "dirLight.specular"), 1, &specularColor[0]);

		lightColor.x = 0.8f + 0.2f * sin(curTime * 1.0f);
		lightColor.y = 0.1f + 0.1f * sin(curTime * 0.5f);
		lightColor.z = 0.1f + 0.1f * sin(curTime * 1.5f);
		diffuseColor = lightColor * glm::vec3(0.5f);
		ambientColor = diffuseColor * glm::vec3(0.2f);
		specularColor = lightColor;
		glUniform3fv(glGetUniformLocation(objectShader.Program, "pointLights[0].position"), 1, &pointLightsPos[0][0]);
		glUniform3fv(glGetUniformLocation(objectShader.Program, "pointLights[0].ambient"), 1, &ambientColor[0]);
		glUniform3fv(glGetUniformLocation(objectShader.Program, "pointLights[0].diffuse"), 1, &diffuseColor[0]);
		glUniform3fv(glGetUniformLocation(objectShader.Program, "pointLights[0].specular"), 1, &specularColor[0]);
		glUniform1f(glGetUniformLocation(objectShader.Program, "pointLights[0].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(objectShader.Program, "pointLights[0].linear"), 0.09f);
		glUniform1f(glGetUniformLocation(objectShader.Program, "pointLights[0].quadratic"), 0.032f);

		lightColor.x = 0.1f + 0.1f * sin(curTime * 1.0f);
		lightColor.y = 0.8f + 0.2f * sin(curTime * 0.5f);
		lightColor.z = 0.1f + 0.1f * sin(curTime * 1.5f);
		diffuseColor = lightColor * glm::vec3(0.5f);
		ambientColor = diffuseColor * glm::vec3(0.2f);
		specularColor = lightColor;
		glUniform3fv(glGetUniformLocation(objectShader.Program, "pointLights[1].position"), 1, &pointLightsPos[1][0]);
		glUniform3fv(glGetUniformLocation(objectShader.Program, "pointLights[1].ambient"), 1, &ambientColor[0]);
		glUniform3fv(glGetUniformLocation(objectShader.Program, "pointLights[1].diffuse"), 1, &diffuseColor[0]);
		glUniform3fv(glGetUniformLocation(objectShader.Program, "pointLights[1].specular"), 1, &specularColor[0]);
		glUniform1f(glGetUniformLocation(objectShader.Program, "pointLights[1].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(objectShader.Program, "pointLights[1].linear"), 0.09f);
		glUniform1f(glGetUniformLocation(objectShader.Program, "pointLights[1].quadratic"), 0.032f);

		lightColor.x = 0.2f + 0.2f * sin(curTime * 1.0f);
		lightColor.y = 0.2f + 0.2f * sin(curTime * 0.5f);
		lightColor.z = 0.8f + 0.2f * sin(curTime * 1.5f);
		diffuseColor = lightColor * glm::vec3(0.5f);
		ambientColor = diffuseColor * glm::vec3(0.2f);
		specularColor = lightColor;
		glUniform3fv(glGetUniformLocation(objectShader.Program, "pointLights[2].position"), 1, &pointLightsPos[2][0]);
		glUniform3fv(glGetUniformLocation(objectShader.Program, "pointLights[2].ambient"), 1, &ambientColor[0]);
		glUniform3fv(glGetUniformLocation(objectShader.Program, "pointLights[2].diffuse"), 1, &diffuseColor[0]);
		glUniform3fv(glGetUniformLocation(objectShader.Program, "pointLights[2].specular"), 1, &specularColor[0]);
		glUniform1f(glGetUniformLocation(objectShader.Program, "pointLights[2].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(objectShader.Program, "pointLights[2].linear"), 0.09f);
		glUniform1f(glGetUniformLocation(objectShader.Program, "pointLights[2].quadratic"), 0.032f);

		glStencilFunc(GL_ALWAYS, 1, 0xFF);
		glStencilMask(0xFF);

		glm::mat4 model(1.0f);
		for (size_t i = 0; i < 10; ++i) {
			model = glm::translate(glm::mat4(1.0f), uniquePositions[i]);
			GLfloat angle = curTime * glm::radians(20.0f) + 20.0f * i;
			model = glm::rotate(model, angle, glm::vec3(0.5f, 0.5f, 0.0f));
			model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));			

			glUniformMatrix4fv(glGetUniformLocation(objectShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));

			glBindVertexArray(VAO);
			glDrawArrays(GL_TRIANGLES, 0, 12);
			glBindVertexArray(0);
		}


		// Plane floor
		//glStencilFunc(GL_ALWAYS, 1, 0xFF);
		glStencilMask(0x00);
		objectShader.Use();
		glBindVertexArray(planeVAO);
	    glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, floorMap);

		glUniform1i(glGetUniformLocation(objectShader.Program, "material.diffuse"), 3);
		glUniform1i(glGetUniformLocation(objectShader.Program, "material.specular"), 3);

		model = glm::mat4(1.0f);
		glUniformMatrix4fv(glGetUniformLocation(objectShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(glGetUniformLocation(objectShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(objectShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);


		// Point lighters
		glStencilMask(0x00);
		lightShader.Use();
		for (size_t i = 0; i < 3; ++i) {
			model = glm::translate(glm::mat4(1.0f), pointLightsPos[i]);
			model = glm::scale(model, glm::vec3(0.2f));
			glUniformMatrix4fv(glGetUniformLocation(lightShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
			glUniformMatrix4fv(glGetUniformLocation(lightShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
			glUniformMatrix4fv(glGetUniformLocation(lightShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

			lightColor.x = 0.1f + 0.1f * sin(curTime * 1.0f);
			lightColor.y = 0.1f + 0.1f * sin(curTime * 0.5f);
			lightColor.z = 0.1f + 0.1f * sin(curTime * 1.5f);
			lightColor[i] += 0.8f;
			glUniform3fv(glGetUniformLocation(lightShader.Program, "lightColor"), 1, &lightColor[0]);

			glBindVertexArray(lightVAO);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		

		// Direct lighter
		model = glm::translate(glm::mat4(1.0f), -sunLightDir);
		//model = glm::scale(model, glm::vec3(0.2f));
		glUniformMatrix4fv(glGetUniformLocation(lightShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(glGetUniformLocation(lightShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(lightShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

		lightColor = glm::vec3(1.0f);
		glUniform3fv(glGetUniformLocation(lightShader.Program, "lightColor"), 1, &lightColor[0]);

		glBindVertexArray(lightVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);


		// Borders
		glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
		glStencilMask(0x00);
		glDisable(GL_DEPTH_TEST);
		borderShader.Use();
		glUniformMatrix4fv(glGetUniformLocation(borderShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(borderShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		glUniform3fv(glGetUniformLocation(borderShader.Program, "viewPos"), 1, &camera.Position[0]);
		glUniform1f(glGetUniformLocation(borderShader.Program, "curTime"), curTime);
		glUniform3fv(glGetUniformLocation(borderShader.Program, "borderColor"), 1, &borderColor[0]);
		GLfloat borderScale = 1.1f;
		if (borderColor.x > 0.01f) {
			for (size_t i = 0; i < 10; ++i) {
				model = glm::translate(glm::mat4(1.0f), uniquePositions[i]);
				GLfloat angle = curTime * glm::radians(20.0f) + 20.0f * i;
				model = glm::rotate(model, angle, glm::vec3(0.5f, 0.5f, 0.0f));
				model = glm::scale(model, glm::vec3(borderScale));

				glUniformMatrix4fv(glGetUniformLocation(borderShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));

				glBindVertexArray(VAO);
				glDrawArrays(GL_TRIANGLES, 0, 12);
				glBindVertexArray(0);
			}
		}
		glStencilMask(0xFF);
		glStencilFunc(GL_ALWAYS, 1, 0xFF);
		glEnable(GL_DEPTH_TEST);


		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &lightVAO);
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(2, VBO);
	glfwTerminate();

	return 0;
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		camera.ProcessKeyboard(Camera_Movement::FORWARD, deltaTime);
	} else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		camera.ProcessKeyboard(Camera_Movement::BACKWARD, deltaTime);
	} else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		camera.ProcessKeyboard(Camera_Movement::LEFT, deltaTime);
	} else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		camera.ProcessKeyboard(Camera_Movement::RIGHT, deltaTime);
	} else if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
		borderColor = glm::vec3(0.78f, 0.1f, 0.52f);
	} else if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
		borderColor = glm::vec3(0.0f);
	}
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse) {
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // y-координаты снизу вверх

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}

unsigned int loadTexture(const char *path)
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

