#include <iostream>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <SOIL/SOIL.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Shaders/shader.h"


GLfloat mixValue = 0.2f;


void key_callback(GLFWwindow* window, int key,
	int scancode, int action, int mode) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GL_TRUE);
	} else if (key == GLFW_KEY_UP && action == GLFW_PRESS) {
		mixValue += 0.1f;
		mixValue = mixValue >= 1.0f ? 1.0f : mixValue;
	} else if (key == GLFW_KEY_DOWN && action == GLFW_PRESS) {
		mixValue -= 0.1f;
		mixValue = mixValue <= 0.0f ? 0.0f : mixValue;
	}
}


// Window dimensions
const GLuint WIDTH = 1400, HEIGHT = 700;




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

	glEnable(GL_DEPTH_TEST);

	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);

	glfwSetKeyCallback(window, key_callback);

	// Shaders
	Shader ourShaderGrad("../LibStuff/Include/Shaders/src/vshader1.vsh", "../LibStuff/Include/Shaders/src/fshader1.fsh");

	// Vertices
	GLfloat vertTriangle[] = {
		-0.5f, -0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   0.0f, 0.0f,
		0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   4.0f, 0.0f,
		0.0f,  0.5f, 0.0f,   0.3f, 0.3f, 0.3f,   2.0f, 4.0f,

		0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   0.0f, 0.0f,
		0.0f, -0.5f, -0.5f,   0.0f, 0.0f, 1.0f,   4.0f, 0.0f,
		0.0f,  0.5f, 0.0f,   0.3f, 0.3f, 0.3f,   2.0f, 4.0f,

		0.0f, -0.5f, -0.5f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,
		-0.5f, -0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   4.0f, 0.0f,
		0.0f,  0.5f, 0.0f,   0.3f, 0.3f, 0.3f,   2.0f, 4.0f,

		-0.5f, -0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   0.0f, 0.0f,
		0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   4.0f, 0.0f,
		0.0f, -0.5f, -0.5f,   0.0f, 0.0f, 1.0f,   2.0f, 4.0f

	};


	GLuint VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertTriangle),
		vertTriangle, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 
            8 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
		    8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE,
		    8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);
	glBindVertexArray(0);


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
		glfwPollEvents();
		// Rendering
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		// Draw the first triangle using the data from our first VAO
		ourShaderGrad.Use();
		
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture1);
		glUniform1i(glGetUniformLocation(ourShaderGrad.Program, "ourTexture1"), 0);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture2);
		glUniform1i(glGetUniformLocation(ourShaderGrad.Program, "ourTexture2"), 1);
		
		for (size_t i = 0; i < 10; ++i) {
			glm::mat4 model = glm::translate(glm::mat4(1.0f), uniquePositions[i]); 
			GLfloat angle = (GLfloat)glfwGetTime() * glm::radians(50.0f) + 20.0f * i;
			model = glm::rotate(model, angle, glm::vec3(0.5f, 0.5f, 0.0f));
			model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.5f));
			glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -5.0f));
			glm::mat4 projection = glm::perspective(glm::radians(45.0f), (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 100.0f);

			GLuint transLoc = glGetUniformLocation(ourShaderGrad.Program, "model");
			glUniformMatrix4fv(transLoc, 1, GL_FALSE, glm::value_ptr(model));
			transLoc = glGetUniformLocation(ourShaderGrad.Program, "view");
			glUniformMatrix4fv(transLoc, 1, GL_FALSE, glm::value_ptr(view));
			transLoc = glGetUniformLocation(ourShaderGrad.Program, "projection");
			glUniformMatrix4fv(transLoc, 1, GL_FALSE, glm::value_ptr(projection));
			glUniform1f(glGetUniformLocation(ourShaderGrad.Program, "mixValue"), mixValue);

			glBindVertexArray(VAO);
			glDrawArrays(GL_TRIANGLES, 0, 12);
			glBindVertexArray(0);
		}

		/*
		// Draw the first triangle using the data from our first VAO
		ourShaderGrad.Use();

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture1);
		glUniform1i(glGetUniformLocation(ourShaderGrad.Program, "ourTexture1"), 0);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture2);
		glUniform1i(glGetUniformLocation(ourShaderGrad.Program, "ourTexture2"), 1);

		trans = glm::mat4(1.0f);
		GLfloat curTime = glfwGetTime();
		trans = glm::translate(trans, glm::vec3(-0.5f, -0.5f, 0.0f));
		trans = glm::scale(trans, glm::vec3(0.7f + 0.3f * sin(4 * curTime), 0.7f + 0.3f * cos(4 * curTime), 1.0f));
		//trans = glm::rotate(trans, (float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));
		transLoc = glGetUniformLocation(ourShaderGrad.Program, "transform");
		glUniformMatrix4fv(transLoc, 1, GL_FALSE, glm::value_ptr(trans));

		glUniform1f(glGetUniformLocation(ourShaderGrad.Program, "mixValue"), mixValue);

		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glBindVertexArray(0);
		*/



		glfwSwapBuffers(window);
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glfwTerminate();
	

/*	glm::mat4 model = glm::scale(glm::mat4(1.0f), glm::vec3(2.0f, 2.0f, 1.0f));
	model = glm::translate(model, glm::vec3(1.0f, -1.0f, 0.0f));
	//model = glm::rotate(model, glm::radians(45.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	glm::vec4 v(1.0f, 1.0f, 1.0f, 1.0f);
	v = model * v;
	std::cout << v.x << " " << v.y << " " << v.z << " " << v.w << std::endl;
	*/

	return 0;
}
