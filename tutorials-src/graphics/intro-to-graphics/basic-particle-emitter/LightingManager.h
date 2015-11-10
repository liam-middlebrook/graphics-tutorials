#pragma once
#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

static const unsigned int MAX_LIGHTS = 8;

struct Light
{
	glm::vec3 position;
	glm::vec3 linearVelocity;
	glm::quat rotation;
	glm::vec3 rotationOrigin;
	glm::quat angularVelocity;
	glm::vec4 color;
	glm::vec4 transformPos;
	GLfloat diffusePower;
};

struct UniformLight
{
	glm::vec4 position;
	glm::vec4 color_difPower;
};

struct LightsBlock
{
	UniformLight lights[MAX_LIGHTS];
	glm::vec4 ambient;
};

class LightingManager
{
public:
	static void Init();
	static void Update(float dt);
	static void DumpData();
	static Light* GetLight(int index);
	static void SetAmbient(glm::vec3 color);
private:
	static Light _lights[MAX_LIGHTS];
	static GLfloat _lightBufferData[MAX_LIGHTS * 8];

	static LightsBlock _lightsData;
};
