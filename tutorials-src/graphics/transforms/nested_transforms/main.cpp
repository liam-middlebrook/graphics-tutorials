/*
Nested Transforms
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
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <iostream>

GLFWwindow* window;

GLuint vertexShader;
GLuint fragmentShader;
GLuint shaderProgram;

GLuint triangleBuffer;
GLint posAttrib;
GLint uTransform;

GLfloat vertices[] = {
	+0.0f, +0.433f,
	-0.25f, 0.0f,
	+0.25f, 0.0f
};

struct Transform
{
	glm::vec3 position;
	glm::vec3 rotationOrigin;
	glm::quat rotation;
	glm::vec3 scale;
	glm::vec3 scaleOrigin;

	glm::vec3 linearVelocity;
	glm::quat angularVelocity;

	glm::mat4 transformMat;

	Transform* parent;

	Transform()
	{
		position = glm::vec3();
		rotationOrigin = glm::vec3();
		rotation = glm::quat();
		scale = glm::vec3(1.0f, 1.0f, 1.0f);
		scaleOrigin = glm::vec3();

		linearVelocity = glm::vec3();
		angularVelocity = glm::quat();

		transformMat = glm::mat4();

		parent = (Transform*)nullptr;
	}
};

Transform parentTriangle;
Transform childTriangle;
Transform grandchildTriangle;

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

	window = glfwCreateWindow(800, 600, "Nested_Transforms", NULL, NULL); // Windowed

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

	glfwSetTime(0.0f);

	// Set up the triangles

	parentTriangle = Transform();
	parentTriangle.angularVelocity = glm::angleAxis(10.0f, glm::vec3(0.0f, 0.0f, 1.0f));

	childTriangle = Transform();
	childTriangle.position.x = 0.5f;
	childTriangle.parent = &parentTriangle;
	childTriangle.scale = glm::vec3(0.5f, 0.5f, 0.5f);
	childTriangle.angularVelocity = glm::angleAxis(-25.0f, glm::vec3(0.0f, 0.0f, 1.0f));

	grandchildTriangle = Transform();
	grandchildTriangle.position.x = 0.5f;
	grandchildTriangle.parent = &childTriangle;
	grandchildTriangle.scale = glm::vec3(0.5f, 0.5f, 0.5f);
	grandchildTriangle.angularVelocity = glm::angleAxis(100.0f, glm::vec3(0.0f, 0.0f, 1.0f));

}

void update(Transform &transform, float dt)
{
	// Update values
	transform.position += transform.linearVelocity * dt;
	transform.rotation = glm::slerp(transform.rotation, transform.rotation * transform.angularVelocity, dt);
}

void draw(Transform &transform)
{
	// Apply transforms
	glm::mat4 translateMat = glm::translate(glm::mat4(), transform.position);

	glm::mat4 rotateOriginMat = glm::translate(glm::mat4(), transform.rotationOrigin);
	glm::mat4 rotateMat = rotateOriginMat * glm::mat4_cast(transform.rotation) * glm::inverse(rotateOriginMat);

	glm::mat4 scaleOriginMat = glm::translate(glm::mat4(), transform.scaleOrigin);
	glm::mat4 scaleMat = scaleOriginMat * glm::scale(glm::mat4(), transform.scale) * glm::inverse(scaleOriginMat);

	glm::mat4 parentTransformMat = transform.parent ? transform.parent->transformMat : glm::mat4();

	transform.transformMat = parentTransformMat * (translateMat * rotateMat * scaleMat);

	glUniformMatrix4fv(uTransform, 1, GL_FALSE, glm::value_ptr(transform.transformMat));

	//Make draw call
	glDrawArrays(GL_TRIANGLES, 0, 3);
}

void step()
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
	glfwSetTime(0.0f);

	update(parentTriangle, dt);
	update(childTriangle, dt);
	update(grandchildTriangle, dt);

	// The parent must be drawn first in this implementation because their
	// transforms matrices are calculated when they are drawn.
	draw(parentTriangle);
	draw(childTriangle);
	draw(grandchildTriangle);

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
		step();
		glfwPollEvents();
	}

	cleanUp();

	return 0;
}
