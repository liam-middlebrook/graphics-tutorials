#pragma once
#include <GL/glew.h>
#include <vector>

class RenderObject;
struct Mesh;
struct Shader;

class RenderManager
{
public:
	static void Init(unsigned int numRenderObjects = 0);
	static RenderObject* InitRenderObject(Mesh* mesh, GLint shader, GLenum mode = GL_TRIANGLES, GLuint layer = 1);
	static void Update(float dt);

	static void Draw(GLuint mask);
	static void DumpData();
private:
	static std::vector<RenderObject*> _displayList;
	static unsigned int _displayListLength;
};
