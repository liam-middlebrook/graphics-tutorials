/*
Multi Buffer
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
#include <iostream>

GLFWwindow* window;

GLuint vertexShader;
GLuint fragmentShader;
GLuint shaderProgram;

GLuint triangleBufferA;
GLuint triangleBufferB;
GLint posAttrib;

GLfloat verticesA[] = {
	-0.55f, -0.433f,
	-0.8f, 0.0f,
	-0.3f, 0.0f
};
GLfloat verticesB[] = {
	+0.55f, +0.433f,
	+0.8f, 0.0f,
	+0.3f, 0.0f
};

const char* vertexSource =
"#version 150\n"
"in vec2 position;"
"void main()"
"{"
"gl_Position = vec4(position, 0.0, 1.0);"
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
}

int init()
{
	if (!glfwInit())
    {
        return 1;
    }

	//Create window
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);

	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	window = glfwCreateWindow(800, 600, "Multi_Buffer", NULL, NULL); // Windowed

	//Activate window
	glfwMakeContextCurrent(window);

	//Hook up to gpu
	glewExperimental = GL_TRUE;
	glewInit();

	//Store the data for the triangles in 2 buffers that the gpu can use to draw
	glGenBuffers(1, &triangleBufferA);
	glBindBuffer(GL_ARRAY_BUFFER, triangleBufferA);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verticesA), verticesA, GL_STATIC_DRAW);

	glGenBuffers(1, &triangleBufferB);
	glBindBuffer(GL_ARRAY_BUFFER, triangleBufferB);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verticesB), verticesB, GL_STATIC_DRAW);

	initShaders();

    return 0;
}

void draw()
{
	//Clear to black
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Bind the vertex buffer containing the two triangles
	glBindBuffer(GL_ARRAY_BUFFER, triangleBufferA);
	//Indicate how the data should be used in the shader
	glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0);

	//Draw the triangles
	glDrawArrays(GL_TRIANGLES, 0, 6);

	//Bind the vertex buffer containing the two triangles
	glBindBuffer(GL_ARRAY_BUFFER, triangleBufferB);
	//Indicate how the data should be used in the shader
	glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0);

	//Draw the triangles
	glDrawArrays(GL_TRIANGLES, 0, 6);

	//Swap buffers
	glfwSwapBuffers(window);
}

void cleanUp()
{
	glDeleteProgram(shaderProgram);
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	glDeleteBuffers(1, &triangleBufferA);
	glDeleteBuffers(1, &triangleBufferB);
}

int main()
{
	if(init())
    {
        return 1;
    }

	while (!glfwWindowShouldClose(window))
	{
		draw();
		glfwPollEvents();
	}

	cleanUp();

	return 0;
}
