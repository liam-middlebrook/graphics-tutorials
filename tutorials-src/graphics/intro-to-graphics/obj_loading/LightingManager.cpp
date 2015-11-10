#include "LightingManager.h"
#include "ResourceManager.h"
#include "RenderManager.h"
#include <glm/gtc/type_ptr.hpp>
#include <string>

Light LightingManager::_lights[MAX_LIGHTS];
LightsBlock LightingManager::_lightsData;

void LightingManager::Init()
{
	for (int i = 0; i < MAX_LIGHTS; ++i)
	{
		_lights[i] = Light();
		_lights[i].position = glm::vec3();
		_lights[i].linearVelocity = glm::vec3();
		_lights[i].rotation = glm::quat();
		_lights[i].rotationOrigin = glm::vec3();
		_lights[i].angularVelocity = glm::quat();
		_lights[i].transformPos = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		_lights[i].color = glm::vec4();
		_lights[i].diffusePower = 0.0f;
	}	

	SetAmbient(glm::vec3(0.5f, 0.5f, 0.5f));
}
void LightingManager::Update(float dt)
{
	GLsizei lightSize = sizeof(GLfloat) * 8;
	GLsizei vec4Size = sizeof(GLfloat) * 4;
	for (int i = 0; i < MAX_LIGHTS; ++i)
	{
		_lights[i].position += _lights[i].linearVelocity * dt;
		_lights[i].rotation = glm::slerp(_lights[i].rotation, _lights[i].rotation * _lights[i].angularVelocity, dt);

		glm::mat4 translateMat = glm::translate(glm::mat4(), _lights[i].position);

		glm::mat4 rotateOriginMat = translateMat * glm::translate(glm::mat4(), _lights[i].rotationOrigin);
		glm::mat4 rotateMat = glm::inverse(rotateOriginMat) * glm::mat4_cast(_lights[i].rotation) * rotateOriginMat;

		_lights[i].transformPos = translateMat * rotateMat * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

		// Update the uniform buffer values
		_lightsData.lights[i].position = glm::vec4(_lights[i].position, 1.0f);
		_lightsData.lights[i].color_difPower = glm::vec4(_lights[i].color.xyz(), _lights[i].diffusePower);

		memcpy(ResourceManager::lightsBuffer.data + lightSize * i, glm::value_ptr(_lightsData.lights[i].position), vec4Size);
		memcpy(ResourceManager::lightsBuffer.data + lightSize * i + vec4Size, glm::value_ptr(_lightsData.lights[i].color_difPower), vec4Size);
	}
	// Send the unform buffer data to the buffer location
	glBindBuffer(GL_UNIFORM_BUFFER, ResourceManager::lightsBuffer.bufferLocation);
	glBufferData(GL_UNIFORM_BUFFER, ResourceManager::lightsBuffer.size, ResourceManager::lightsBuffer.data, GL_DYNAMIC_DRAW);
}

void LightingManager::DumpData()
{
	
}

Light* LightingManager::GetLight(int index)
{
	return &_lights[index];
}

void LightingManager ::SetAmbient(glm::vec3 color)
{
	_lightsData.ambient = glm::vec4(color, 1.0f);
	memcpy(ResourceManager::lightsBuffer.data + sizeof(GLfloat) * 8 * MAX_LIGHTS, glm::value_ptr(_lightsData.ambient), sizeof(GLfloat) * 4);
}

	
