#include <iostream>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Shaders/shader.h"


void key_callback(GLFWwindow* window, int key,
	int scancode, int action, int mode) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
}


// Window dimensions
const GLuint WIDTH = 800, HEIGHT = 600;

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	// Window
	GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "TrialWindow", 
		                                  nullptr, nullptr);
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

	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);

	glfwSetKeyCallback(window, key_callback);

	// Shaders
	Shader ourShaderGrad("C:/Users/HP/source/repos/graphics2020/LibStuff/Include/Shaders/src/vshader1.vsh", "C:/Users/HP/source/repos/graphics2020/LibStuff/Include/Shaders/src/fshader1.frag");
	Shader ourShaderChange("C:/Users/HP/source/repos/graphics2020/LibStuff/Include/Shaders/src/vshader2.vsh", "C:/Users/HP/source/repos/graphics2020/LibStuff/Include/Shaders/src/fshader2.frag");


	// Vertices
	GLfloat vertTriangle[] = {
	    -0.5f,  0.0f, 0.0f,  1.0f, 0.0f, 0.0f,
	     0.0f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,
	    -0.5f, -0.5f, 0.0f,  0.0f, 0.0f, 1.0f
	};

	GLfloat vertRectangle[] = {
	    0.5f, 0.5f, 0.0f,  // Верхний правый угол
	    0.5f, 0.0f, 0.0f,  // Нижний правый угол
	    0.0f, 0.0f, 0.0f,  // Нижний левый угол
	    0.0f, 0.5f, 0.0f   // Верхний левый угол
	};

	GLuint indices[] = {  // Помните, что мы начинаем с 0!
	    0, 1, 3,   // Первый треугольник
	    1, 2, 3    // Второй треугольник
	};

	GLuint VBO[2], VAO[2];
	glGenVertexArrays(2, VAO);
	glGenBuffers(2, VBO);

	glBindVertexArray(VAO[0]);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertTriangle),
		vertTriangle, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 
            6 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
		    6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	glBindVertexArray(0);

	GLuint EBO;
	glGenBuffers(1, &EBO);
	glBindVertexArray(VAO[1]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertRectangle),
		vertRectangle, GL_STATIC_DRAW);
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices),
		indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
		3 * sizeof(GLfloat), (GLvoid*)0);

	glEnableVertexAttribArray(0);
	glBindVertexArray(0);

	// Run

	// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	// glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		// Rendering
		glClearColor(0.2f, 0.8f, 0.9f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		
		// Draw the first triangle using the data from our first VAO
		ourShaderGrad.Use();
		glBindVertexArray(VAO[0]);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		
		ourShaderChange.Use();
		GLfloat timeValue = glfwGetTime();
		GLfloat greenValue = (sin(timeValue) / 2) + 0.5;
		glUniform4f(glGetUniformLocation(ourShaderChange.Program, "ourColor"), 0.0f, greenValue, 0.0f, 1.0f);		
		glBindVertexArray(VAO[1]);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		glBindVertexArray(0);

		glfwSwapBuffers(window);
	}

	glDeleteVertexArrays(2, VAO);
	glDeleteBuffers(2, VBO);
	glDeleteBuffers(1, &EBO);
	glfwTerminate();
	return 0;
}
