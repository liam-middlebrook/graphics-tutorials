/*
Title: Instancing
File Name: main.cpp
Copyright © 2015
Original authors: Benjamin Robbins
Written under the supervision of David I. Schwartz, Ph.D., and
supported by a professional development seed grant from the B. Thomas
Golisano College of Computing & Information Sciences
(https://www.rit.edu/gccis) at the Rochester Institute of Technology.

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or (at
your option) any later version.

This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

Description:
Introduces the concept of Instancing where you draw the same set of coordinates from the Vertex Buffer, 
only changing the the transformation matrix uniform. This allows you draw the same set of geometry several 
times using the same buffers, only changing the location via the transformation matrix. 
*/
#define GLM_FORCE_RADIANS
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/ext.hpp>
#include <iostream>

GLFWwindow* window;

GLuint vertexShader;
GLuint fragmentShader;
GLuint shaderProgram;

GLuint triangleBuffer;
GLint posAttrib;
GLint uTransform;

// The positions of the two triangles 
// When using instanced based rendering, the same vertex information is
// reused multiple times. Instead, individual shapes are managed by their transforms
glm::vec3 positionA = glm::vec3(-0.5f, 0.0f, 0.0f);
glm::vec3 positionB = glm::vec3(+0.5f, 0.0f, 0.0f);

GLfloat vertices[] = {
	+0.0f, +0.433f,
	-0.25f, 0.0f,
	+0.25f, 0.0f
};

const char* vertexSource =
"#version 150\n"
"in vec2 position;"
"uniform mat4 transform;"
// This is a uniform value. It's basically used to communicate information
// to the shader. The difference between uniform values and the "in" values
// is the way that they are used. "in" values represent the input data for 
// this particular shader call. In this case it is the location of this
// vertex, so it will be different every time. Uniform values use data 
// that is constant for each individual shader call, thus "uniform". This 
// is useful for applying a mathematical change to an entire shape. In 
// this case, we're using it to apply a transform to the triangle.
"void main()"
"{"
"gl_Position = transform * vec4(position, 0.0, 1.0);"
"}";

const char* fragmentSource =
"#version 150\n"
"out vec4 outColor;"
"void main()"
"{"
"outColor = vec4(1.0, 1.0, 1.0, 1.0);"
"}";


void initShaders()
{
	// Compile the vertex shader
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexSource, NULL);
	glCompileShader(vertexShader);

	// Compile the fragment shader
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
	glCompileShader(fragmentShader);

	// Combine shaders into a program, bind, link and use
	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);

	glBindFragDataLocation(shaderProgram, 0, "outColor");

	glLinkProgram(shaderProgram);

	glUseProgram(shaderProgram);

	// Bind buffer data to shader values
	posAttrib = glGetAttribLocation(shaderProgram, "position");
	glEnableVertexAttribArray(posAttrib);

	uTransform = glGetUniformLocation(shaderProgram, "transform");
}

void init()
{
	if (!glfwInit()) exit(EXIT_FAILURE);

	//Create window
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);

	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	window = glfwCreateWindow(800, 600, "Instancing", NULL, NULL); // Windowed

	//Activate window
	glfwMakeContextCurrent(window);

	//Hook up to gpu
	glewExperimental = GL_TRUE;
	glewInit();

	// Compile shaders
	initShaders();

	// Store the data for the triangles in a buffer that the gpu can use to draw
	glGenBuffers(1, &triangleBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, triangleBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
}

void draw()
{
	// Clear to black
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Bind the vertex buffer containing the triangle data
	glBindBuffer(GL_ARRAY_BUFFER, triangleBuffer);
	// Indicate how the data should be used in the shader
	glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0);

	// Update and draw the first triangle
	glm::mat4 transform = glm::translate(positionA);
	glUniformMatrix4fv(uTransform, 1, GL_FALSE, glm::value_ptr(transform));
	glDrawArrays(GL_TRIANGLES, 0, 3);

	// Update and draw the second triangle
	transform = glm::translate(positionB);
	glUniformMatrix4fv(uTransform, 1, GL_FALSE, glm::value_ptr(transform));
	glDrawArrays(GL_TRIANGLES, 0, 3);

	// Swap buffers
	glfwSwapBuffers(window);
}

void cleanUp()
{
	glDeleteProgram(shaderProgram);
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	glDeleteBuffers(1, &triangleBuffer);

	glfwTerminate();
}

int main()
{
	init();

	while (!glfwWindowShouldClose(window))
	{
		draw();
		glfwPollEvents();
	}

	cleanUp();

	return 0;
}
