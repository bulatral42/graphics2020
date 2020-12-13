#include <iostream>
#include <map>
#include <vector>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <SOIL/SOIL.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <Shaders/shader.h>
#include <camera.h>
#include <stb/stb_image.h>


// Window dimensions
const GLuint WIDTH = 1400, HEIGHT = 800;
int effectFlag = 0;

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

void processInput(GLFWwindow *window);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
GLuint loadTexture(const char *path);
GLuint loadCubeMap(std::vector<std::string> faces);


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
	Shader screenShader("../LibStuff/Include/Shaders/src/vscreen.vsh", "../LibStuff/Include/Shaders/src/fscreen.fsh");
	Shader skyboxShader("../LibStuff/Include/Shaders/src/vskybox.vsh", "../LibStuff/Include/Shaders/src/fskybox.fsh");

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
		0.0f, -0.5f, -0.5f,   0.0f, 0.0f, 1.0f,   2.0f, 4.0f,  0.0f, 1.0f, 0.0f,
		0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   4.0f, 0.0f,  0.0f, 1.0f, 0.0f
		
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
		-10.0f, -3.0f, -10.0f,  0.0f, 2.0f,  0.0f, 1.0f, 0.0f,
		-10.0f, -3.0f,  10.0f,  0.0f, 0.0f,  0.0f, 1.0f, 0.0f,

		 10.0f, -3.0f,  10.0f,  2.0f, 0.0f,  0.0f, 1.0f, 0.0f,
		 10.0f, -3.0f, -10.0f,  2.0f, 2.0f,  0.0f, 1.0f, 0.0f,
		-10.0f, -3.0f, -10.0f,  0.0f, 2.0f,  0.0f, 1.0f, 0.0f
		 
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


	GLfloat vertScreen[] = {
		-1.0f, -1.0f, 0.0f,  0.0f, 0.0f,
		 1.0f, -1.0f, 0.0f,  1.0f, 0.0f,
		 1.0f,  1.0f, 0.0f,  1.0f, 1.0f,

		-1.0f, -1.0f, 0.0f,  0.0f, 0.0f,
		 1.0f,  1.0f, 0.0f,  1.0f, 1.0f,
		-1.0f,  1.0f, 0.0f,  0.0f, 1.0f
    };

	GLuint scrVAO, scrVBO;
	glGenVertexArrays(1, &scrVAO);
	glGenBuffers(1, &scrVBO);

	glBindBuffer(GL_ARRAY_BUFFER, scrVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertScreen), &vertScreen, GL_STATIC_DRAW);

	glBindVertexArray(scrVAO);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));

	glBindVertexArray(0);


	GLfloat vertSkybox[] = {
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

	GLuint skyboxVAO, skyboxVBO;
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);

	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertSkybox), &vertSkybox, GL_STATIC_DRAW);

	glBindVertexArray(skyboxVAO);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);

	glBindVertexArray(0);


	// Framebuffer
	GLuint FBO;
	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	GLuint textureColorBuffer;
	glGenTextures(1, &textureColorBuffer);
	glBindTexture(GL_TEXTURE_2D, textureColorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WIDTH, HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorBuffer, 0);

	GLuint RBO;
	glGenRenderbuffers(1, &RBO);
	glBindRenderbuffer(GL_RENDERBUFFER, RBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, WIDTH, HEIGHT);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	std::vector<std::string> skyfacesSea {
		"evening_right.jpg",
		"evening_left.jpg",
		"evening_top.jpg",
		"evening_bot.jpg",
		"evening_front.jpg",
		"evening_back.jpg"
		
	};
	std::vector<std::string> skyfacesSpace {
		"bkg1_right.png",
		"bkg1_left.png",
		"bkg1_top.png",
		"bkg1_bot.png",
		"bkg1_front.png",
		"bkg1_back.png"

	};

	GLuint diffuseMap = loadTexture("container_diff.png");
	GLuint specularMap = loadTexture("container_spec.png");
	GLuint emissionMap = loadTexture("container_emission.jpg");
	GLuint floorMap = loadTexture("floor_laminate.jpg");
	GLuint windowMap = loadTexture("window_red.png");
	GLuint skyboxTexture = loadCubeMap(skyfacesSpace);


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
	
	std::vector<glm::vec3> windowPos {
		glm::vec3(-8.0f, 0.0f, -1.48f),
		glm::vec3(-9.0f, 0.0f, -0.51f),
		glm::vec3(-7.0f, 0.0f, -0.90f),
		glm::vec3(4.0f, 0.0f, 0.9f),
		glm::vec3(6.0f, 0.0f, 0.51f),
		glm::vec3(7.0f, 0.0f, 0.7f)
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
		glBindFramebuffer(GL_FRAMEBUFFER, FBO);
		
		GLfloat curTime = (GLfloat)glfwGetTime();
		deltaTime = curTime - lastTime;
		lastTime = curTime;
		

		processInput(window);
		// Rendering
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 100.0f);

		glStencilMask(0x00);
		glDisable(GL_CULL_FACE); 
		glDepthMask(GL_FALSE);
		skyboxShader.Use();

		glm::mat4 viewSky = glm::mat4(glm::mat3(camera.GetViewMatrix()));
		glUniformMatrix4fv(glGetUniformLocation(skyboxShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(viewSky));
		glUniformMatrix4fv(glGetUniformLocation(skyboxShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);
		glUniform1i(glGetUniformLocation(skyboxShader.Program, "skyboxTexture"), 0);

		glBindVertexArray(skyboxVAO);
		glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glDepthMask(GL_TRUE);


		
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
		glEnable(GL_CULL_FACE);
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
		glDisable(GL_CULL_FACE);
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

			lightColor.r = 0.1f + 0.1f * sin(curTime * 1.0f);
			lightColor.g = 0.1f + 0.1f * sin(curTime * 0.5f);
			lightColor.b = 0.1f + 0.1f * sin(curTime * 1.5f);
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



		// Windows
		objectShader.Use();
		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, windowMap);

		glUniform1i(glGetUniformLocation(objectShader.Program, "material.diffuse"), 4);
		glUniform1i(glGetUniformLocation(objectShader.Program, "material.ambient"), 4);
		glUniform1i(glGetUniformLocation(objectShader.Program, "material.specular"), 4);

		std::map<GLfloat, glm::vec3> sortedWindows;
		for (size_t i = 0; i < windowPos.size(); ++i) {
			GLfloat distance = glm::length(camera.Position - windowPos[i]);
			sortedWindows[distance] = windowPos[i];
		}
		glBindVertexArray(planeVAO); 
		for (auto it = sortedWindows.rbegin(); it != sortedWindows.rend(); ++it) {
			model = glm::mat4(1.0f);
			model = glm::translate(model, it->second);
			model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f)); 
			model = glm::scale(model, glm::vec3(0.1f));
			glUniformMatrix4fv(glGetUniformLocation(objectShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
			glDrawArrays(GL_TRIANGLES, 0, 6);
		}
		glBindVertexArray(0);


		// Borders
		glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
		glStencilMask(0x00);
		glDisable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
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
		glDisable(GL_CULL_FACE);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		screenShader.Use();
		glActiveTexture(GL_TEXTURE5);
		glBindTexture(GL_TEXTURE_2D, textureColorBuffer);
		glUniform1i(glGetUniformLocation(screenShader.Program, "screenTexture"), 5);
		glUniform1i(glGetUniformLocation(screenShader.Program, "effectFlag"), effectFlag);
		glBindVertexArray(scrVAO);
		glDisable(GL_DEPTH_TEST);
		glDrawArrays(GL_TRIANGLES, 0, 6);
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

void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		camera.ProcessKeyboard(Camera_Movement::FORWARD, 2.0f * deltaTime);
	} else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		camera.ProcessKeyboard(Camera_Movement::BACKWARD, 2.0f * deltaTime);
	} else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		camera.ProcessKeyboard(Camera_Movement::LEFT, 2.0f * deltaTime);
	} else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		camera.ProcessKeyboard(Camera_Movement::RIGHT, 2.0f * deltaTime);
	} else if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
		borderColor = glm::vec3(0.78f, 0.1f, 0.52f);
	} else if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
		borderColor = glm::vec3(0.0f);
	} else if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS) {
		effectFlag = 0;
	} else if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS) {
		effectFlag = 1;
	} else if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS) {
		effectFlag = 2;
	} else if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS) {
		effectFlag = 3;
	} else if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS) {
		effectFlag = 4;
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

GLuint loadTexture(const char *path)
{
	GLuint textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
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

GLuint loadCubeMap(std::vector<std::string> faces)
{
	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, nrChannels;
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
			);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Cubemap tex failed to load at path: " << faces[i] << std::endl;
			stbi_image_free(data);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureID;
}
