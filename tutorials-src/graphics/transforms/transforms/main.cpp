/*
Transforms
(c) 2015
original authors: Benjamin Robbins
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
*/
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/ext.hpp>
#include <iostream>

GLFWwindow* window;

GLuint vertexShader;
GLuint fragmentShader;
GLuint shaderProgram;

GLuint triangleBuffer;
GLint posAttrib;
GLint uTransform;

glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
// Rotation may occur around any arbitrary point in space 
glm::vec3 rotationOrigin = glm::vec3(0.0f, 0.433f, 0.0f);
// Any given state of rotation is traditionaly represented by a four value quaternion
// lots of complex math, imaginary numbers, hyperdimensional stuff
glm::quat rotation = glm::quat();
glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);
glm::vec3 scaleOrigin = glm::vec3(0.0f, 0.216f, 0.0f);

glm::vec3 linearVelocity = glm::vec3(0.1f, 0.0f, 0.0f);
glm::vec4 angularVelocity = glm::vec4(0.0f, 0.0f, 1.0f, 180.0f);
glm::vec3 scaleRate = glm::vec3(0.1f, 0.1f, 0.1f);

GLfloat vertices[] = {
	+0.0f, +0.433f,
	-0.25f, 0.0f,
	+0.25f, 0.0f
};

const char* vertexSource =
"#version 150\n"
"in vec2 position;"
"uniform mat4 transform;"
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

	window = glfwCreateWindow(800, 600, "Transforms", NULL, NULL); // Windowed

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

	glfwSetTime(0.0);
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

	// Get delta time since the last frame
	float dt = (float)glfwGetTime();
	glfwSetTime(0.0);

	// Update values
	position += linearVelocity * dt;
	glm::quat dRot = glm::angleAxis(angularVelocity.w * dt, glm::vec3(angularVelocity.x, angularVelocity.y, angularVelocity.z));
	rotation = rotation * dRot;
	scale += scaleRate * dt;

	// Apply transforms
	glm::mat4 translateMat = glm::translate(position);

	glm::mat4 rotateOriginMat = glm::translate(rotationOrigin);
	glm::mat4 rotateMat = rotateOriginMat * glm::mat4_cast(rotation) * glm::inverse(rotateOriginMat);

	glm::mat4 scaleOriginMat = glm::translate(scaleOrigin);
	glm::mat4 scaleMat = scaleOriginMat * glm::scale(scale) * glm::inverse(scaleOriginMat);

	glm::mat4 transform = translateMat * rotateMat * scaleMat;

	glUniformMatrix4fv(uTransform, 1, GL_FALSE, glm::value_ptr(transform));
	
	// Make draw call
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
