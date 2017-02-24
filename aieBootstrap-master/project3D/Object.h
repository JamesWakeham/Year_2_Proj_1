#pragma once
#include "Application3D.h"
#include <gl_core_4_4.h>

class Object
{
	GLint textureLocation;

	//Model* model;

	char* textureName;

public:
	Object();
	~Object();

	unsigned int m_VAO;
	unsigned int m_VBO;
	unsigned int m_IBO;

	void Update(float deltaTime);
	void Draw(unsigned int programID);

};

