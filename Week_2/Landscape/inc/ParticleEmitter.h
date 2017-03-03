#pragma once
#include <glm/glm.hpp>
#include "gl_core_4_4.h"
#include "Shader.h"
class ParticleEmitter
{
public:
	ParticleEmitter(const char* fileName) : m_particles(nullptr),
		m_firstDead(0),
		m_maxParticles(0),
		m_position(0, 0, 0),
		m_vao(0), m_vbo(0), m_ibo(0),
		m_vertexData(nullptr) {
		m_shader = new Shader(fileName);
	}
	~ParticleEmitter() {
		delete[] m_particles;
		delete[] m_vertexData;
		glDeleteVertexArrays(1, &m_vao);
		glDeleteBuffers(1, &m_vbo);
		glDeleteBuffers(1, &m_ibo);
	}
	void Init(unsigned int a_maxParticles,
		unsigned int a_emitRate,
		float a_lifetimeMin, float a_lifetimeMax,
		float a_velocityMin, float a_velocityMax,
		float a_startSize, float a_endSize,
		const glm::vec4& a_startColour, const glm::vec4& a_endColour,glm::vec3 pos);
	void Emit();
	void Draw(glm::mat4 projectionView);
	void Update(float a_deltaTime,const glm::mat4& a_cameraTransform, glm::vec3 camPos);
	void DrawUI();
protected:
	struct Particle {
		glm::vec3 position;
		glm::vec3 velocity;
		glm::vec4 colour;
		float size;
		float lifetime;
		float lifespan;
	};
	struct ParticleVertex {
		glm::vec4 position;
		glm::vec4 colour;
	};
	Particle* m_particles;
	Shader* m_shader;
	unsigned int m_firstDead;
	unsigned int m_maxParticles;
	unsigned int m_vao, m_vbo, m_ibo;
	ParticleVertex* m_vertexData;

	glm::vec3 m_position;
	float m_emitTimer;
	float m_emitRate;
	float m_lifespanMin;
	float m_lifespanMax;
	float m_velocityMin;
	float m_velocityMax;
	float m_startSize;
	float m_endSize;
	glm::vec4 m_startColour;
	glm::vec4 m_endColour;
	
	// more to come here!
};

