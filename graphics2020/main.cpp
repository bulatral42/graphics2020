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


// Window dimensions
const GLuint WIDTH = 1400, HEIGHT = 800;

GLfloat mixValue = 0.2f;
glm::vec3 cameraPos(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp(0.0f, 1.0f, 0.0f);
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = WIDTH / 2.0f;
float lastY = HEIGHT / 2.0f;
bool firstMouse = true;

glm::vec3 lightPos(1.5f, 1.0f, 2.0f);
glm::vec3 lightColorWhite(1.0f, 1.0f, 1.0f);
glm::vec3 lightColorRed(1.0f, 0.0f, 0.0f);
glm::vec3 lightColorGreen(0.0f, 1.0f, 0.0f);
glm::vec3 lightColorBlue(0.0f, 0.0f, 1.0f);


void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}

	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
		mixValue += deltaTime;
		mixValue = mixValue >= 1.0f ? 1.0f : mixValue;
	} else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
		mixValue -= deltaTime;
		mixValue = mixValue <= 0.0f ? 0.0f : mixValue;
	} else if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		camera.ProcessKeyboard(Camera_Movement::FORWARD, deltaTime);
	} else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		camera.ProcessKeyboard(Camera_Movement::BACKWARD, deltaTime);
	} else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		camera.ProcessKeyboard(Camera_Movement::LEFT, deltaTime);
	} else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		camera.ProcessKeyboard(Camera_Movement::RIGHT, deltaTime);
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

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	// Window
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, 
		    "WhrillingTetraedr", nullptr, nullptr);
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
	
	glEnable(GL_DEPTH_TEST);

	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);

	// Shaders
	Shader objectShader("../LibStuff/Include/Shaders/src/vshader3.vsh", "../LibStuff/Include/Shaders/src/fshader3.fsh");
	Shader lightShader("../LibStuff/Include/Shaders/src/vlight1.vsh", "../LibStuff/Include/Shaders/src/flight1.fsh");

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
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertTriangle),
		vertTriangle, GL_STATIC_DRAW);
	
	glBindVertexArray(VAO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 
            11 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
		    11 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE,
		    11 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE,
		11 * sizeof(GLfloat), (GLvoid*)(8 * sizeof(GLfloat)));
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


	GLuint texture1, texture2;
	glGenTextures(1, &texture1);
	glBindTexture(GL_TEXTURE_2D, texture1);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	int textWidth, textHeight;
	unsigned char* image = SOIL_load_image("container.jpg", &textWidth, &textHeight, 0, SOIL_LOAD_RGB);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, textWidth, textHeight,
	    	0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0);


	glGenTextures(1, &texture2);
	glBindTexture(GL_TEXTURE_2D, texture2);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	image = SOIL_load_image("awesomeface.png", &textWidth, &textHeight, 0, SOIL_LOAD_RGB);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, textWidth, textHeight,
		0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0);

	glm::vec3 uniquePositions[] = {
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


	// Run
	// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	// glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	while (!glfwWindowShouldClose(window)) {
		GLfloat curTime = (GLfloat)glfwGetTime();
		deltaTime = curTime - lastTime;
		lastTime = curTime;

		processInput(window); 
		// Rendering
		//glClearColor(0.8f, 0.1f, 0.6f, 1.0f);
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		objectShader.Use();
		
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture1);
		glUniform1i(glGetUniformLocation(objectShader.Program, "ourTexture1"), 0);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture2);
		glUniform1i(glGetUniformLocation(objectShader.Program, "ourTexture2"), 1);

		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 100.0f);

		GLuint transLoc = glGetUniformLocation(objectShader.Program, "view");
		glUniformMatrix4fv(transLoc, 1, GL_FALSE, glm::value_ptr(view));
		transLoc = glGetUniformLocation(objectShader.Program, "projection");
		glUniformMatrix4fv(transLoc, 1, GL_FALSE, glm::value_ptr(projection));

		glUniform1f(glGetUniformLocation(objectShader.Program, "mixValue"), mixValue);
		glUniform3fv(glGetUniformLocation(objectShader.Program, "lightColor"), 1, &lightColorWhite[0]);
		glUniform3fv(glGetUniformLocation(objectShader.Program, "lightPos"), 1, &lightPos[0]);
		glUniform3fv(glGetUniformLocation(objectShader.Program, "viewPos"), 1, &camera.Position[0]);

		glm::mat4 model(1.0f);

		for (size_t i = 0; i < 10; ++i) {
			model = glm::translate(glm::mat4(1.0f), uniquePositions[i]);
			GLfloat angle = curTime * glm::radians(25.0f) + 20.0f * i;
			model = glm::rotate(model, angle, glm::vec3(0.5f, 0.5f, 0.0f));
			model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.5f));			

			transLoc = glGetUniformLocation(objectShader.Program, "model");
			glUniformMatrix4fv(transLoc, 1, GL_FALSE, glm::value_ptr(model));

			glBindVertexArray(VAO);
			glDrawArrays(GL_TRIANGLES, 0, 12);
			glBindVertexArray(0);
		}

		lightShader.Use();
		model = glm::translate(glm::mat4(1.0f), lightPos);
		model = glm::scale(model, glm::vec3(0.2f));
		//view = glm::mat4(1.0f);
		//projection = glm::mat4(1.0f);

		transLoc = glGetUniformLocation(lightShader.Program, "view");
		glUniformMatrix4fv(transLoc, 1, GL_FALSE, glm::value_ptr(view));
		transLoc = glGetUniformLocation(lightShader.Program, "projection");
		glUniformMatrix4fv(transLoc, 1, GL_FALSE, glm::value_ptr(projection));
		transLoc = glGetUniformLocation(lightShader.Program, "model");
		glUniformMatrix4fv(transLoc, 1, GL_FALSE, glm::value_ptr(model));


		glBindVertexArray(lightVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &lightVAO);
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(2, VBO);
	glfwTerminate();

	return 0;
}
