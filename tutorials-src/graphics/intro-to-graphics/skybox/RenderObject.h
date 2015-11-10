#pragma once
#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

struct Mesh;
struct CameraBlock;

struct PerModelBlock
{
	glm::mat4 modelMat;
	glm::mat4 invTransModelMat;
	glm::vec4 color;
};

struct Transform
{
	glm::vec3 position;
	glm::quat rotation;
	glm::vec3 scale;
	glm::vec3 linearVelocity;
	glm::quat angularVelocity;
	glm::vec3 rotationOrigin;
	glm::mat4 translate;
	glm::mat4 model;
	Transform* parent;
};

class RenderObject
{
public:
	RenderObject(Mesh* mesh = nullptr, GLint shader = -1, GLenum mode = GL_TRIANGLES, GLuint layer = 1);
	~RenderObject();

	void Update(float dt);
	void Draw();

	Transform& transform();
	void setColor(glm::vec4 color);
	bool active();
	void active(bool isActive);
	bool useDepthTest();
	void useDepthTest(bool usingDepth);
	GLenum mode();
	void mode(GLenum newMode);
	GLuint layer();
	void layer(GLuint newLayer);
	GLuint texture();
	void texture(GLuint newTexture);
private:
	Mesh* _mesh;
	GLint _shader;
	GLuint _texture;
	PerModelBlock _perModelBlock;
	GLuint _perModelBufferLocation;
	Transform _transform;
	GLenum _mode;
	GLuint _layer;
};
