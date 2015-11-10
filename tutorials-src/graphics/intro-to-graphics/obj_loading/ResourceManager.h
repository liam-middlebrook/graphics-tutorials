#pragma once
#include "GL/glew.h"
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

struct Vertex
{
	float posX;
	float posY;
	float posZ;
	float texCoordU;
	float texCoordV;
	float normX;
	float normY;
	float normZ;
};

struct Mesh
{
	GLuint vao;
	GLuint vbo;
	GLfloat* vertexBuffer;
	GLint vertexBufferSize;
	GLuint ebo;
	GLint* elementBuffer;
	GLint count;
};

struct UniformBuffer
{
	GLuint size;
	GLuint bufferLocation;
	GLubyte* data;
};

class ResourceManager
{
public:
	static void Init();
	static void DumpData();

	static GLint phongShader;
	static GLuint phongFragShader;
	static GLuint phongVertShader;
	static UniformBuffer perModelBuffer;
	static UniformBuffer cameraBuffer;
	static UniformBuffer lightsBuffer;

	static Mesh sphere;
	static Mesh cube;
	static Mesh plane;

private:
	static char* ReadTextFile(const char* filepath);
	static GLuint CompileShader(char* shader, GLenum type);
	static GLuint LinkShaderProgram(GLuint* shaders, int numShaders, GLuint fragDataBindColorNumber, char* fragDataBindName);
	static void LoadOBJ(char* obj, Mesh& mesh, GLint shader);
	static void ParseOBJ(char* obj, std::vector<GLfloat>* vertPos, std::vector<GLfloat>* vertNorm, std::vector<GLfloat>* texCoord, std::vector<GLint>* elements);
	static void GenMesh(GLfloat* verts, GLint vertsLength, GLint* elements, GLint count, Mesh& mesh, GLint shader);
	static void GenVertices(std::vector<GLfloat>* verts, std::vector<GLint>* vertElements, std::vector<GLfloat>* vertPos, std::vector<GLfloat>* vertNorms, std::vector<GLfloat>* texCoord, std::vector<GLint>* elements);
	static void GenUniformBuffer(UniformBuffer& buffer, GLsizei bufferSize);
	static void ReleaseMesh(Mesh& mesh);
	static void ReleaseBuffer(UniformBuffer& buffer);
};
