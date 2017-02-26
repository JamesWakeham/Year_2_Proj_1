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

class Application3D : public aie::Application {
public:
	//const static Application3D* instance;
	// our vertex and index buffers 
	unsigned int m_VAO;
	unsigned int m_VBO;
	unsigned int m_IBO;

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

	virtual bool startup();
	virtual void shutdown();

	void CreateShaders();
	void CreateTextureShader();
	void CreateParticleShader();

	void GenerateGrid(unsigned int rows, unsigned int cols);
	void GenerateTextureGrid();

	void DrawGrid(unsigned int rows, unsigned int cols);
	void DrawGridWithTexture();

	void LoadImageTextures();

	void LoadAllModels();

	void LoadObj();
	void LoadObj(const char* location);

	void CreateOpenGlBuffers(tinyobj::attrib_t & attribs, std::vector<tinyobj::shape_t>& shapes);

	virtual void update(float deltaTime);
	virtual void draw();

	unsigned int GetProgromID();
protected:

	glm::mat4	m_viewMatrix;
	glm::mat4	m_projectionMatrix;
};