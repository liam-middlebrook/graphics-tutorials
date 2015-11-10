#include "ResourceManager.h"
#include "LightingManager.h"
#include <fstream>
#include <iostream>
#include <string.h>

GLint ResourceManager::phongShader;
GLuint ResourceManager::phongVertShader;
GLuint ResourceManager::phongFragShader;

UniformBuffer ResourceManager::perModelBuffer;
UniformBuffer ResourceManager::cameraBuffer;
UniformBuffer ResourceManager::lightsBuffer;

const GLuint PERMODEL_BIND_POINT = 0;
const GLuint LIGHTS_BIND_POINT = 1;
const GLuint CAMERA_BIND_POINT = 2;

Mesh ResourceManager::sphere;
Mesh ResourceManager::cube;
Mesh ResourceManager::plane;


void ResourceManager::Init()
{
	GLuint shaders[2];
	
	phongFragShader = CompileShader("fshader.glsl", GL_FRAGMENT_SHADER);
	phongVertShader = CompileShader("vshader.glsl", GL_VERTEX_SHADER);

	shaders[0] = phongFragShader;
	shaders[1] = phongVertShader;

	phongShader = LinkShaderProgram(shaders, 2, 0, "outColor");

	GLsizei bufferSize;
	GLuint uPerModelBlockIndex = glGetUniformBlockIndex(phongShader, "perModel");
	glUniformBlockBinding(phongShader, uPerModelBlockIndex, PERMODEL_BIND_POINT);
	glGetActiveUniformBlockiv(phongShader, uPerModelBlockIndex, GL_UNIFORM_BLOCK_DATA_SIZE, &bufferSize);
	GenUniformBuffer(perModelBuffer, bufferSize);
	glBindBufferBase(GL_UNIFORM_BUFFER, PERMODEL_BIND_POINT, perModelBuffer.bufferLocation);

	GLuint uCameraBlockIndex = glGetUniformBlockIndex(phongShader, "camera");
	glUniformBlockBinding(phongShader, uCameraBlockIndex, CAMERA_BIND_POINT);
	glGetActiveUniformBlockiv(phongShader, uCameraBlockIndex, GL_UNIFORM_BLOCK_DATA_SIZE, &bufferSize);
	GenUniformBuffer(cameraBuffer, bufferSize);
	glBindBufferBase(GL_UNIFORM_BUFFER, CAMERA_BIND_POINT, cameraBuffer.bufferLocation);

	GLuint uLightsBlockIndex = glGetUniformBlockIndex(phongShader, "lightsBlock");
	glUniformBlockBinding(phongShader, uLightsBlockIndex, LIGHTS_BIND_POINT);
	glGetActiveUniformBlockiv(phongShader, uLightsBlockIndex, GL_UNIFORM_BLOCK_DATA_SIZE, &bufferSize);
	GenUniformBuffer(lightsBuffer, bufferSize);
	glBindBufferBase(GL_UNIFORM_BUFFER, LIGHTS_BIND_POINT, lightsBuffer.bufferLocation);

	LoadOBJ("Sphere.obj", sphere, phongShader);
	LoadOBJ("Cube.obj", cube, phongShader);
	LoadOBJ("Plane.obj", plane, phongShader);
}

void ResourceManager::DumpData()
{
	ReleaseMesh(sphere);
	ReleaseMesh(cube);
	ReleaseMesh(plane);
	glDeleteShader(phongFragShader);
	glDeleteShader(phongVertShader);
	glDeleteProgram(phongShader);

	ReleaseBuffer(perModelBuffer);
	ReleaseBuffer(cameraBuffer);
	ReleaseBuffer(lightsBuffer);
}

char* ResourceManager::ReadTextFile(const char* filepath)
{
	FILE* file;
	char* content = NULL;

	int count = 0;

	if (filepath != NULL)
	{
		file = fopen(filepath, "rt");

		if (file != NULL)
		{
			fseek(file, 0, SEEK_END);
			count = ftell(file);
			rewind(file);

			if (count > 0)
			{
				content = new char[count + 1];
				count = fread(content, sizeof(char), count, file);
				content[count] = '\0';
			}
			fclose(file);
		}
	}
	return content;
}

GLuint ResourceManager::CompileShader(char* shader, GLenum type)
{
	//Create the shader
	GLuint retShader;

	//get a shader handler
	retShader = glCreateShader(type);
	//read the shader from the source file
	const char* shaderSource = ReadTextFile(shader);
	//pass source to GL
	glShaderSource(retShader, 1, &shaderSource, NULL);
	//delete the memory from the source text
	delete[] shaderSource;
	//Compile shader
	glCompileShader(retShader);

	//Check compilation errors
	GLint  compiled;
	glGetShaderiv(retShader, GL_COMPILE_STATUS, &compiled);
	if (!compiled)
	{
		std::cerr << shader << " failed to compile:" << std::endl;
		GLint  logSize;
		glGetShaderiv(retShader, GL_INFO_LOG_LENGTH, &logSize);
		char* logMsg = new char[logSize];
		glGetShaderInfoLog(retShader, logSize, NULL, logMsg);
		std::cerr << logMsg << std::endl;
		delete[] logMsg;
		system("pause");
		exit(EXIT_FAILURE);
	}
	return retShader;
}

GLuint ResourceManager::LinkShaderProgram(GLuint* shaders, int numShaders, GLuint fragDataBindColorNumber, char* fragDataBindName)
{
	GLuint program = glCreateProgram();

	for (int i = 0; i < numShaders; ++i)
	{
		glAttachShader(program, shaders[i]);
	}

	//Tell the fragment shader which buffer to write to
	glBindFragDataLocation(program, fragDataBindColorNumber, fragDataBindName);

	glLinkProgram(program);

	//Check linking errors
	GLint  linked;
	glGetProgramiv(program, GL_LINK_STATUS, &linked);
	if (!linked)
	{
		std::cerr << "Shader program failed to link" << std::endl;
		GLint  logSize;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logSize);
		char* logMsg = new char[logSize];
		glGetProgramInfoLog(program, logSize, NULL, logMsg);
		std::cerr << logMsg << std::endl;
		delete[] logMsg;

		exit(EXIT_FAILURE);
	}

	return program;
}

void ResourceManager::LoadOBJ(char* obj, Mesh& mesh, GLint shader)
{
	std::vector<GLfloat> vertPos = std::vector<GLfloat>();
	std::vector<GLfloat> vertNorms = std::vector<GLfloat>();
	std::vector<GLfloat> texCoord = std::vector<GLfloat>();
	std::vector<GLint> elements = std::vector<GLint>();
	ParseOBJ(ReadTextFile(obj), &vertPos, &vertNorms, &texCoord, &elements);

	std::vector<GLfloat> verts = std::vector<GLfloat>();
	std::vector<GLint> vertElements = std::vector<GLint>();
	GenVertices(&verts, &vertElements, &vertPos, &vertNorms, &texCoord, &elements);

	GenMesh(&verts[0], verts.size(), &vertElements[0], vertElements.size(), mesh, shader);
}

float StringToFloat(const char* string)
{
	bool decimalPointHit = false;
	bool negative = false;
	float decimalFactor = 1.0f;
	float retFloat = 0.0f;
	int stringItr = 0;
	while (string[stringItr])
	{
		if (string[stringItr] == '-')
		{
			negative = true;
		}
		else if (string[stringItr] == '.')
		{
			decimalPointHit = true;
		}
		else
		{
			float value = (float)(string[stringItr] - 48);
			if (decimalPointHit)
			{
				decimalFactor *= 0.1f;
				retFloat += value * decimalFactor;
			}
			else
			{
				retFloat = retFloat * 10.0f + value;
			}
		}
		++stringItr;
	}
	if (negative) retFloat *= -1.0f;

	return retFloat;
}

int StringToInt(const char* string)
{
	int retInt = 0;
	int stringItr = 0;
	int value;
	while (string[stringItr])
	{
		value = string[stringItr] - 48;
		retInt = retInt * 10 + value;
		++stringItr;
	}
	return retInt;
}

void ParseFace(std::vector<GLint>* elements, std::vector<char*>& terms)
{
	unsigned int term, termItr, currentComponent, size;
	char *componentStrings[3];
	GLint componentInts[3], *pivot, *prevVert;
	std::vector<GLint*> currentElements = std::vector<GLint*>();
	currentElements.reserve(4);

	// Face, store in elements
	// Parse each of the terms in the terms array by '\' and store the resulting value in elements
	componentStrings[0] = nullptr;	// Positions
	componentStrings[1] = nullptr;	// Tex Coordinates
	componentStrings[2] = nullptr;	// Normals
	size = terms.size();
	for (term = 1; term < size; ++term)
	{
		// A single face contains one or more triangles, 3 elements need to be added for each triangle
		currentComponent = 0;
		termItr = 0;
		componentStrings[0] = &terms[term][0];
		for (termItr = 0; terms[term][termItr]; ++termItr)
		{
			if (terms[term][termItr] == '/')
			{
				terms[term][termItr] = '\0';
				componentStrings[++currentComponent] = &terms[term][termItr + 1];
			}
		}
		componentInts[0] = StringToInt(componentStrings[0]);
		componentInts[1] = StringToInt(componentStrings[1]);
		componentInts[2] = StringToInt(componentStrings[2]);
		currentElements.push_back(new GLint[3]);
		memcpy(currentElements[term - 1], componentInts, sizeof(GLint) * 3);
	}
	size = currentElements.size();
	pivot = nullptr;
	prevVert = nullptr;
	// Add all of the elemnts in currentElements as triangle adjacencies
	for (unsigned int i = 0; i < size; ++i)
	{
		if (!pivot)
		{
			pivot = currentElements[i];
		}
		else if (!prevVert)
		{
			prevVert = currentElements[i];
		}
		else
		{
			//A
			elements->push_back(pivot[0]);
			elements->push_back(pivot[1]);
			elements->push_back(pivot[2]);
			//B
			elements->push_back(prevVert[0]);
			elements->push_back(prevVert[1]);
			elements->push_back(prevVert[2]);
			//C
			elements->push_back(currentElements[i][0]);
			elements->push_back(currentElements[i][1]);
			elements->push_back(currentElements[i][2]);
			prevVert = currentElements[i];
		}
	}

	for (unsigned int i = 0; i < size; ++i)
	{
		delete[] currentElements[i];
	}
}

void ResourceManager::ParseOBJ(char* obj, std::vector<GLfloat>* vertPos, std::vector<GLfloat>* vertNorm, std::vector<GLfloat>* texCoord, std::vector<GLint>* elements)
{
	vertPos->reserve(1024);
	vertNorm->reserve(1024);
	texCoord->reserve(1024);
	elements->reserve(2048);
	unsigned int lineItr;
	char currentChar, line[256];
	std::vector<char*> terms = std::vector<char*>();
	terms.reserve(5);
	for (int i = 0; obj[i]; ++i)
	{
		currentChar = obj[i];
		if (currentChar != '\n')
		{
			// Get the line we're on and put it in its own string
			for (int j = 0;; ++j, ++i)
			{
				currentChar = obj[i];
				if (currentChar != '\n' && currentChar)
				{
					line[j] = currentChar;
				}
				else
				{
					line[j] = '\0';
					break;
				}
			}
			// If the line starts with '#' then it is a comment and should be ignored
			if (line[0] == '#')
			{
				continue;
			}

			// Parse the line by ' ' and put each value into its own string
			lineItr = 0;
			terms.push_back(line);
			while (line[lineItr])
			{
				if (line[lineItr] == ' ')
				{
					line[lineItr] = '\0';
					terms.push_back(&line[lineItr + 1]);
				}
				lineItr++;
			}

			// The first term will determine what type of data follows
			if (terms[0][0] == 'v'&& !terms[0][1])
			{
				// Vertex store in the vertPos vector
				vertPos->push_back(StringToFloat(terms[1]));
				vertPos->push_back(StringToFloat(terms[2]));
				vertPos->push_back(StringToFloat(terms[3]));
			}

			if (terms[0][0] == 'v' && terms[0][1] == 'n')
			{
				// Normal, store in vertNorm
				vertNorm->push_back(StringToFloat(terms[1]));
				vertNorm->push_back(StringToFloat(terms[2]));
				vertNorm->push_back(StringToFloat(terms[3]));
			}

			if (terms[0][0] == 'v' && terms[0][1] == 't')
			{
				// Texture coordinate, stor in texCoord vector
				texCoord->push_back(StringToFloat(terms[1]));
				texCoord->push_back(StringToFloat(terms[2]));
			}

			if (terms[0][0] == 'f')
			{
				ParseFace(elements, terms);
			}

			terms.clear();
		}
	}

	delete[] obj;
}

void ResourceManager::GenMesh(GLfloat* verts, GLint vertsLength, GLint* elements, GLint count, Mesh& mesh, GLint shader)
{
	mesh = Mesh();

	mesh.vertexBuffer = new GLfloat[vertsLength];
	memcpy(mesh.vertexBuffer, verts, sizeof(GLfloat) * vertsLength);
	mesh.vertexBufferSize = vertsLength;

	mesh.elementBuffer = new GLint[count];
	memcpy(mesh.elementBuffer, elements, sizeof(GLfloat) * count);
	mesh.count = count;

	glGenVertexArrays(1, &mesh.vao);
	glBindVertexArray(mesh.vao);

	glGenBuffers(1, &mesh.vbo);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertsLength, mesh.vertexBuffer, GL_STATIC_DRAW);

	glGenBuffers(1, &mesh.ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLint) * count, mesh.elementBuffer, GL_STATIC_DRAW);

	GLint posAttrib = glGetAttribLocation(shader, "position");
	glEnableVertexAttribArray(posAttrib);
	glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), 0);

	GLint texAttrib = glGetAttribLocation(shader, "texCoord");
	glEnableVertexAttribArray(texAttrib);
	glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));

	GLint normAttrib = glGetAttribLocation(shader, "normal");
	glEnableVertexAttribArray(normAttrib);
	glVertexAttribPointer(normAttrib, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(5 * sizeof(GLfloat)));
}

void ResourceManager::GenVertices(std::vector<GLfloat>* verts, std::vector<GLint>* vertElements, std::vector<GLfloat>* vertPos, std::vector<GLfloat>* vertNorms, std::vector<GLfloat>* texCoord, std::vector<GLint>* elements)
{
	unsigned int numElements = elements->size() / 3;
	vertElements->resize(numElements);
	// Initialize to -1
	for (unsigned int i = 0; i < numElements; ++i)
	{
		(*vertElements)[i] = -1;
	}

	unsigned int uniqueElementCount = 0;
	bool commonVert, commonUV, commonNorm;
	for (unsigned int i = 0; i < numElements; ++i)
	{
		if ((*vertElements)[i] == -1)
		{
			(*vertElements)[i] = uniqueElementCount++;
		}
		// Compare each element to all other elements to find common verts
		for (unsigned int j = i + 1; j < numElements; ++j)
		{
			commonVert = (*elements)[i * 3] == (*elements)[j * 3];
			commonUV = (*elements)[i * 3 + 1] == (*elements)[j * 3 + 1];
			commonNorm = (*elements)[i * 3 + 2] == (*elements)[j * 3 + 2];
			if (commonVert && commonUV && commonNorm)
			{
				(*vertElements)[j] = (*vertElements)[i];
			}
		}
	}

	verts->resize(uniqueElementCount * 8);
	unsigned int u, posValueIndex, texCoordValueIndex, normValueIndex;
	u = 0;
	for (unsigned int i = 0; i < numElements; ++i)
	{
		if ((*vertElements)[i] == u)
		{
			posValueIndex = ((*elements)[i * 3] - 1) * 3;
			texCoordValueIndex = ((*elements)[i * 3 + 1] - 1) * 2;
			normValueIndex = ((*elements)[i * 3 + 2] - 1) * 3;

			(*verts)[u * 8] = (*vertPos)[posValueIndex];
			(*verts)[u * 8 + 1] = (*vertPos)[posValueIndex + 1];
			(*verts)[u * 8 + 2] = (*vertPos)[posValueIndex + 2];

			(*verts)[u * 8 + 3] = (*texCoord)[texCoordValueIndex];
			(*verts)[u * 8 + 4] = (*texCoord)[texCoordValueIndex + 1];

			(*verts)[u * 8 + 5] = (*vertNorms)[normValueIndex];
			(*verts)[u * 8 + 6] = (*vertNorms)[normValueIndex + 1];
			(*verts)[u * 8 + 7] = (*vertNorms)[normValueIndex + 2];
			++u;
		}
	}
}

void ResourceManager::GenUniformBuffer(UniformBuffer& buffer, GLsizei bufferSize)
{
	buffer = UniformBuffer();

	buffer.data = new GLubyte[bufferSize];
	buffer.size = bufferSize;
	glGenBuffers(1, &buffer.bufferLocation);
}

void ResourceManager::ReleaseMesh(Mesh& mesh)
{
	if (&mesh)
	{
		glDeleteVertexArrays(1, &mesh.vao);
		glDeleteBuffers(1, &mesh.vbo);
		delete[] mesh.vertexBuffer;
		glDeleteBuffers(1, &mesh.ebo);
		delete[] mesh.elementBuffer;
	}
}

void ResourceManager::ReleaseBuffer(UniformBuffer& buffer)
{
	if (&buffer)
	{
		delete[] buffer.data;
		glDeleteBuffers(1, &buffer.bufferLocation);
	}
}
