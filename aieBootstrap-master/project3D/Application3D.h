#pragma once
#include "MainHeader.h"
#include "tiny_obj_loader.h"
#include "Gizmos.h"
#include "Input.h"
#include "Camera.h"
#include "FlyCam.h"
#include "Entity.h"
#include "Application.h"
#include "ParticleEmitter.h"
#include <imgui.h>

class Application3D : public aie::Application {
public:
	//const static Application3D* instance;
	// our vertex and index buffers 
	unsigned int m_vao;
	unsigned int m_vbo;
	unsigned int m_ibo;
	// the frame buffer object
	unsigned int m_fbo;

	unsigned int m_fboDepth;
	unsigned int m_fboTexture;

	unsigned int m_texture,specMap;
	
	glm::vec3 sunPos;
	glm::vec3 sunCol;

	std::vector<OpenGLInfo> m_glInfo;

	// obj mesh data is stored here after being loaded
	tinyobj::attrib_t attribs;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::shape_t> allShapes;

	glm::mat4 modelPos;

	ParticleEmitter* m_emitter;

	Application3D();
	virtual ~Application3D();

	void FrameBufferStartup();

	virtual bool startup();
	virtual void shutdown();

	void CreateTextureShader();
	void CreateParticleShader();
	void CreateBufferShader();

	void LoadImageTextures();

	void LoadObj();
	void LoadObj(const char* location);

	void CreateOpenGlBuffers(tinyobj::attrib_t & attribs, std::vector<tinyobj::shape_t>& shapes);

	virtual void update(float deltaTime);
	virtual void draw();

	void RenderCulling(Entity entity);

	unsigned int GetProgromID();
protected:

	glm::mat4	m_viewMatrix;
	glm::mat4	m_projectionMatrix;
};