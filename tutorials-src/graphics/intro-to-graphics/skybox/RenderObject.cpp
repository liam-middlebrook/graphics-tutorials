#include "RenderObject.h"
#include "ResourceManager.h"
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include "CameraManager.h"

RenderObject::RenderObject(Mesh* mesh, GLint shader, GLenum mode, GLuint layer)
{
	_mesh = mesh;
	_shader = shader;
	_transform = Transform();
	_transform.position = glm::vec3();
	_transform.rotation = glm::quat();
	_transform.scale = glm::vec3(1.0f, 1.0f, 1.0f);
	_transform.linearVelocity = glm::vec3();
	_transform.angularVelocity = glm::quat();
	_transform.rotationOrigin = glm::vec3();
	_transform.translate = glm::mat4();
	_transform.model = glm::mat4();
	_transform.parent = nullptr;

	_perModelBlock = PerModelBlock();

	_perModelBlock.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

	_mode = mode;
	_layer = layer;
}

RenderObject::~RenderObject()
{
	
}

void RenderObject::Update(float dt)
{
	// Apply velocities
	_transform.position += _transform.linearVelocity;
	_transform.rotation = glm::slerp(_transform.rotation, _transform.rotation * _transform.angularVelocity, dt);

	// Update model matrix
	glm::mat4 parentModel = _transform.parent ? _transform.parent->model : glm::mat4();
	glm::mat4 parentTrans = _transform.parent ? _transform.parent->translate : glm::mat4();

	_transform.translate = glm::translate(glm::mat4(), _transform.position);

	glm::mat4 rotOrigin = glm::translate(glm::mat4(), _transform.rotationOrigin);
	glm::mat4 rotate = glm::inverse(rotOrigin) * glm::mat4_cast(_transform.rotation) * rotOrigin;

	glm::mat4 scale = glm::scale(glm::mat4(), _transform.scale);

	_transform.model = parentModel * (_transform.translate * rotate * scale);

	_perModelBlock.modelMat = _transform.model;
	_perModelBlock.invTransModelMat = glm::transpose(glm::inverse(_transform.model));
}

void RenderObject::Draw()
{
	glBindVertexArray(_mesh->vao);

	glUseProgram(_shader);
	// Update the model Buffer
	GLsizei vec4Size = sizeof(GLfloat) * 4;
	memcpy(ResourceManager::perModelBuffer.data, glm::value_ptr(_perModelBlock.modelMat), vec4Size * 4);
	memcpy(ResourceManager::perModelBuffer.data + vec4Size * 4, glm::value_ptr(_perModelBlock.invTransModelMat), vec4Size * 4);

	memcpy(ResourceManager::perModelBuffer.data + vec4Size * 8, glm::value_ptr(_perModelBlock.color), vec4Size);

	glBindBuffer(GL_UNIFORM_BUFFER, ResourceManager::perModelBuffer.bufferLocation);
	glBufferData(GL_UNIFORM_BUFFER, ResourceManager::perModelBuffer.size, ResourceManager::perModelBuffer.data, GL_DYNAMIC_DRAW);

	glBindTexture(GL_TEXTURE_2D, _texture);
	
	glDrawElements(_mode, _mesh->count, GL_UNSIGNED_INT, 0);
}

Transform& RenderObject::transform() { return _transform; }
void RenderObject::setColor(glm::vec4 color)
{ 
	_perModelBlock.color = color;
}

GLenum RenderObject::mode() { return _mode; }
void RenderObject::mode(GLenum newMode) { _mode = newMode; }
GLuint RenderObject::layer() { return _layer; }
void RenderObject::layer(GLuint newLayer) { _layer = newLayer; }
GLuint RenderObject::texture() { return _texture; }
void RenderObject::texture(GLuint newTexture) { _texture = newTexture; }

