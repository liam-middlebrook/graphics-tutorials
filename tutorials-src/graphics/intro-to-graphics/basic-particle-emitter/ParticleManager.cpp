#include "ParticleManager.h"
#include <glm/gtc/type_ptr.hpp>
#include <random>
#include <glm/gtc/random.hpp>
#include <iostream>

std::vector<ParticleSystem> ParticleManager::_pSystems;

void ParticleManager::Init()
{
	_pSystems = std::vector<ParticleSystem>();
}

void ParticleManager::Update(float dt)
{
	Transform* transform;
	unsigned int size = _pSystems.size();
	for (unsigned int i = 0; i < size; ++i)
	{
		transform = &_pSystems[i].transform;
		// Apply velocities
		transform->position += transform->linearVelocity;
		transform->rotation = glm::slerp(transform->rotation, transform->rotation * transform->angularVelocity, dt);

		// Update model matrix
		glm::mat4 parentModel = transform->parent ? transform->parent->model : glm::mat4();
		glm::mat4 parentTrans = transform->parent ? transform->parent->translate : glm::mat4();

		transform->translate = glm::translate(glm::mat4(), transform->position);

		glm::mat4 rotOrigin = glm::translate(glm::mat4(), transform->rotationOrigin);
		glm::mat4 rotate = glm::inverse(rotOrigin) * glm::mat4_cast(transform->rotation) * rotOrigin;

		glm::mat4 scale = glm::scale(glm::mat4(), transform->scale);

		transform->model = parentModel * (transform->translate * rotate * scale);
		
		ParticleSystem* system = &_pSystems[i];
		system->timeSinceLastEmission += dt;

		// Emit as many particles as need to be emitted based on the frequency of emission.
		while (system->timeSinceLastEmission >= 1.0f / system->frequency)
		{
			system->particles[system->nextAvailableParticle].position_age = transform->model * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
			system->particles[system->nextAvailableParticle].position_age.w = 0.0f;

			system->particles[system->nextAvailableParticle].color = glm::ballRand(0.5f) + glm::vec3(0.5f, 0.5f, 0.5f);

			// Find a random velocity vector within the cone created by the arc of emission attached to the emitter
			float angle = (float)(rand() % system->arc * 1000) / 2000.0f;
			float bearing = (float)(rand() % (int)(2000.0f * 3.1415926535f)) / 1000.0f;
			glm::vec3 axis = glm::vec3(1.0f * cosf(bearing), 0.0f, 1.0f * sinf(bearing));
			glm::vec4 direction = glm::vec4(0.0f, 1.0f, 0.0, 0.0f) * glm::mat4_cast(glm::angleAxis(angle, axis));

			system->particles[system->nextAvailableParticle].velocity = glm::vec3(direction * transform->model * system->initialSpeed);
			system->nextAvailableParticle = (system->nextAvailableParticle + 1) % system->numParticles;

			system->timeSinceLastEmission -= 1.0f / system->frequency;
		}

		for (int i = 0; i < system->numParticles; ++i)
		{
			if (system->particles[i].position_age.w >= 0.0f && system->particles[i].position_age.w < system->lifetime)
			{
				system->particles[i].position_age += glm::vec4(system->particles[i].velocity * dt, dt);
			}
			else if (system->particles[i].position_age.w > system->lifetime)
			{
				system->particles[i].position_age.w = -1.0f;
			}
		}

		memcpy(system->particleBuffer, system->particles, sizeof(Particle) * system->numParticles);

		glBindVertexArray(system->vao);
		glBindBuffer(GL_ARRAY_BUFFER, system->vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Particle) * system->numParticles, system->particleBuffer, GL_DYNAMIC_DRAW);
	}
}

void ParticleManager::Draw()
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDepthMask(GL_FALSE);
	unsigned int size = _pSystems.size();
	for (unsigned int i = 0; i < size; ++i)
	{
		glUseProgram(_pSystems[i].shader);
		glBindVertexArray(_pSystems[i].vao);
		glBindTexture(GL_TEXTURE_2D, _pSystems[i].texture);
		glDrawArrays(GL_POINTS, 0, _pSystems[i].numParticles);
	}
	glDisable(GL_BLEND);
	glDepthMask(GL_TRUE);
}

void ParticleManager::DumpData()
{
	unsigned int size = _pSystems.size();
	for (unsigned int i = 0; i < size; ++i)
	{
		glDeleteBuffers(1, &_pSystems[i].vbo);
		glDeleteVertexArrays(1, &_pSystems[i].vao);
		delete[] _pSystems[i].particles;
		delete[] _pSystems[i].particleBuffer;
	}
}

ParticleSystem* ParticleManager::InitParticleSystem(GLint shader, int numParticles)
{
	unsigned int index = _pSystems.size();
	_pSystems.push_back(ParticleSystem());

	ParticleSystem* system = &_pSystems[index];

	system->transform = Transform();
	system->transform.position = glm::vec3();
	system->transform.rotation = glm::quat();
	system->transform.scale = glm::vec3(1.0f, 1.0f, 1.0f);
	system->transform.linearVelocity = glm::vec3();
	system->transform.angularVelocity = glm::quat();
	system->transform.rotationOrigin = glm::vec3();
	system->transform.translate = glm::mat4();
	system->transform.model = glm::mat4();
	system->transform.parent = nullptr;

	system->shader = shader;

	system->timeSinceLastEmission = 0.0f;
	system->lifetime = 0.0f;
	system->arc = 0;
	system->numParticles = numParticles;
	system->texture = 0;
	system->nextAvailableParticle = 0;
	
	system->particles = new Particle[numParticles];
	for (int i = 0; i < numParticles; ++i)
	{
		system->particles[i].position_age = glm::vec4(0.0f, 0.0f, 0.0f, -1.0f);
		system->particles[i].velocity = glm::vec3();
	}

	system->particleBuffer = new GLubyte[sizeof(Particle) * numParticles];
	memcpy(system->particleBuffer, system->particles, sizeof(Particle) * numParticles);

	glGenVertexArrays(1, &system->vao);
	glBindVertexArray(system->vao);

	glGenBuffers(1, &system->vbo);
	glBindBuffer(GL_ARRAY_BUFFER, system->vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Particle) * numParticles, system->particleBuffer, GL_DYNAMIC_DRAW);
	
	GLuint pos_ageAttrib = glGetAttribLocation(shader, "position_age");
	glEnableVertexAttribArray(pos_ageAttrib);
	glVertexAttribPointer(pos_ageAttrib, 4, GL_FLOAT, GL_FALSE, sizeof(Particle), 0);

	GLuint colorAttrib = glGetAttribLocation(shader, "color");
	glEnableVertexAttribArray(colorAttrib);
	glVertexAttribPointer(colorAttrib, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)(sizeof(GLfloat) * 4));
	
	return system;
}

