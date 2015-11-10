#pragma once
#include <vector>

#include "RenderObject.h"

struct Particle
{
	glm::vec4 position_age;
	glm::vec3 color;
	glm::vec3 velocity;
};

struct ParticleSystem
{
	GLuint vao;
	GLuint vbo;
	Transform transform;
	GLuint texture;
	GLint shader;
	Particle* particles;
	GLubyte* particleBuffer;
	float timeSinceLastEmission;
	int nextAvailableParticle;

	int numParticles; 
	// Length of time before the particle disappears
	float lifetime;
	// Degrees
	int arc;
	// Number of particles emitted per second
	float frequency;
	// The speed of a particle when it is emitted
	float initialSpeed;
};

class ParticleManager
{
public:
	static void Init();
	static void Update(float dt);
	static void Draw();
	static void DumpData();
	static ParticleSystem* InitParticleSystem(GLint shader, int numParticles);
private:
	static std::vector<ParticleSystem> _pSystems;
};