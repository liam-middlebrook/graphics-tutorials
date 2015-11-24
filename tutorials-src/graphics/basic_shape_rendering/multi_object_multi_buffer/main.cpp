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
#include <fstream>

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

// This method reads the text from a file.
// Realistically, we wouldn't want plain text shaders hardcoded in, we'd rather read them in from a separate file so that the shader code is separated.
std::string readShader(std::string fileName)
{
	std::string shaderCode;
	std::string line;

	// We choose ifstream and std::ios::in because we are opening the file for input into our program.
	// If we were writing to the file, we would use ofstream and std::ios::out.
	std::ifstream file(fileName, std::ios::in);

	// This checks to make sure that we didn't encounter any errors when getting the file.
	if (!file.good())
	{
		std::cout << "Can't read file: " << fileName.data() << std::endl;

		// Return so we don't error out.
		return "";
	}

	// ifstream keeps an internal "get" position determining the location of the element to be read next
	// seekg allows you to modify this location, and tellg allows you to get this location
	// This location is stored as a streampos member type, and the parameters passed in must be of this type as well
	// seekg parameters are (offset, direction) or you can just use an absolute (position).
	// The offset parameter is of the type streamoff, and the direction is of the type seekdir (an enum which can be ios::beg, ios::cur, or ios::end referring to the beginning, 
	// current position, or end of the stream).
	file.seekg(0, std::ios::end);					// Moves the "get" position to the end of the file.
	shaderCode.resize((unsigned int)file.tellg());	// Resizes the shaderCode string to the size of the file being read, given that tellg will give the current "get" which is at the end of the file.
	file.seekg(0, std::ios::beg);					// Moves the "get" position to the start of the file.

	// File streams contain two member functions for reading and writing binary data (read, write). The read function belongs to ifstream, and the write function belongs to ofstream.
	// The parameters are (memoryBlock, size) where memoryBlock is of type char* and represents the address of an array of bytes are to be read from/written to.
	// The size parameter is an integer that determines the number of characters to be read/written from/to the memory block.
	file.read(&shaderCode[0], shaderCode.size());	// Reads from the file (starting at the "get" position which is currently at the start of the file) and writes that data to the beginning
	// of the shaderCode variable, up until the full size of shaderCode. This is done with binary data, which is why we must ensure that the sizes are all correct.

	file.close(); // Now that we're done, close the file and return the shaderCode.

	return shaderCode;
}
void initShaders(std::string vert, std::string frag)
{
	const char *vert_txt = vert.c_str();
	const char *frag_txt = frag.c_str();
	const int vert_size = vert.size();
	const int frag_size = frag.size();
	std::cout << vert_txt << std::endl;
	// Compile the vertex shader
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vert_txt, &vert_size);
	glCompileShader(vertexShader);

	// Compile the fragment shaderfrag_size
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &frag_txt, &frag_size);
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

	initShaders(readShader("VertexShader.glsl"), readShader("FragmentShader.glsl"));

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
