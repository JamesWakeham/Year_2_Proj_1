#pragma once

#include "Application.h"
#include <glm/mat4x4.hpp>
#include <vector>
#include "tiny_obj_loader.h"
#include <glm\glm.hpp>
#include <glm\ext.hpp>

struct Vertex { glm::vec4 position; glm::vec4 colour; };
struct OpenGLInfo { unsigned int m_VAO; unsigned int m_VBO; unsigned int m_faceCount; };
struct OBJVertex { float x, y, z; float nx, ny, nz; float u, v; };

class Application3D : public aie::Application {
public:
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

	glm::mat4 modelPos;

	Application3D();
	virtual ~Application3D();

	virtual bool startup();
	virtual void shutdown();

	void CreateShaders();
	void CreateTextureShader();

	void GenerateGrid(unsigned int rows, unsigned int cols);
	void GenerateTextureGrid();

	void DrawGrid(unsigned int rows, unsigned int cols);
	void DrawGridWithTexture();

	void LoadImageTextures();

	void LoadAllModels();

	void LoadObj();

	void CreateOpenGlBuffers(tinyobj::attrib_t & attribs, std::vector<tinyobj::shape_t>& shapes);

	virtual void update(float deltaTime);
	virtual void draw();

	unsigned int GetProgromID();

protected:

	glm::mat4	m_viewMatrix;
	glm::mat4	m_projectionMatrix;
};