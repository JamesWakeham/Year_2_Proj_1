#pragma once
#include "MainHeader.h"

class ParticleEmitter
{
public:
	ParticleEmitter();
	virtual ~ParticleEmitter();
	void Initialise(
		unsigned int a_maxParticles, 
		unsigned int a_emitRate, 
		float a_lifetimeMin, 
		float a_lifetimeMax, 
		float a_velocityMin, 
		float a_velocityMax, 
		float a_startSize, 
		float a_endSize, 
		const  glm::vec4& a_startColour, 
		const  glm::vec4& a_endColour);
	void Initialise(
		unsigned int a_maxParticles,
		unsigned int a_emitRate,
		float a_lifetimeMin,
		float a_lifetimeMax,
		float a_velocityMin,
		float a_velocityMax,
		float a_startSize,
		float a_endSize,
		float a_textureRefNum,
		const  glm::vec4& a_startColour,
		const  glm::vec4& a_endColour,
		const glm::vec3& a_constantForce,
		bool a_rotateToFaceDir
		);

	void Emit();

	void Update(float a_deltaTime, const glm::mat4& a_cameraTransform);

	void Draw(const int shaderID);
	// more to come here

protected:
	Particle* m_particles; 
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
	int m_textureRefNum;
	glm::vec3 m_constantForce;
	glm::vec4 m_startColour;
	glm::vec4 m_endColour;
	bool m_rotateToFaceDir;
	bool m_textured;
};

