#include "CameraManager.h"
#include "ResourceManager.h"
#include "InputManager.h"
#include "RenderObject.h"
#include <GL/glew.h>

glm::mat4 CameraManager::_proj;
glm::mat4 CameraManager::_view;
glm::vec4 CameraManager::_camPos;
glm::vec2 CameraManager::_position;
RenderObject* CameraManager::_skybox;

void CameraManager::Init(float aspectRatio, float fov, float near, float far)
{
	_proj = glm::perspectiveFov(fov, aspectRatio, 1.0f / aspectRatio, near, far);
	_position = glm::vec2(0.0f, 0.0f);
}

void CameraManager::Update(float dt)
{
	float rotRate = 180.0f;
	if (InputManager::cursorLocked())
	{
		glm::vec2 mouseCoords = InputManager::GetMouseCoords();

		_position.x += mouseCoords.x * rotRate;
		_position.x -= _position.x > 360.0f ? 360.0f : 0.0f;

		_position.y += mouseCoords.y * rotRate;
	}

	_position.y = _position.y > 45.0f ? 45.0f : _position.y;
	_position.y = _position.y < -45.0f ? -45.0f : _position.y;

	glm::mat4 bearingMat = glm::mat4_cast(glm::angleAxis(_position.x, glm::vec3(0.0f, 1.0f, 0.0f)));
	glm::mat4 eleMat = glm::mat4_cast(glm::angleAxis(_position.y, glm::vec3(1.0f, 0.0f, 0.0f)));
	glm::mat4 rotMat = eleMat * bearingMat;
	_camPos = glm::vec4(0.0f, 0.0f, -8.0f, 1.0f) * rotMat;

	_view = glm::lookAt(glm::vec3(_camPos), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	GLsizei vec4Size = sizeof(GLfloat) * 4;
	GLsizei mat4Size = vec4Size * 4;
	memcpy(ResourceManager::cameraBuffer.data, glm::value_ptr(_view), vec4Size * 4);
	memcpy(ResourceManager::cameraBuffer.data + mat4Size, glm::value_ptr(_proj), vec4Size * 4);
	memcpy(ResourceManager::cameraBuffer.data + mat4Size * 2, glm::value_ptr(_camPos), vec4Size);

	glBindBuffer(GL_UNIFORM_BUFFER, ResourceManager::cameraBuffer.bufferLocation);
	glBufferData(GL_UNIFORM_BUFFER, ResourceManager::cameraBuffer.size, ResourceManager::cameraBuffer.data, GL_DYNAMIC_DRAW);

	// Update the skybox
	if (_skybox)
	{
		_skybox->transform().position = _camPos.xyz();
		_skybox->transform().rotation = glm::angleAxis(180.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	}
}

void CameraManager::DumpData()
{
	
}

glm::mat4 CameraManager::ProjMat()
{
	return _proj;
}

glm::mat4 CameraManager::ViewMat()
{
	return _view;
}

glm::vec4 CameraManager::CamPos()
{
	return _camPos;
}

RenderObject* CameraManager::Skybox()
{
	return _skybox;
}

void CameraManager::Skybox(RenderObject* newSkybox)
{
	_skybox = newSkybox;
}
