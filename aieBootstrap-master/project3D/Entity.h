#pragma once
#include "MainHeader.h"

using glm::vec3;
using glm::vec4;
using glm::mat4;
class Entity
{
public:
	Entity();
	~Entity();
	Entity(const glm::vec3 startPos);

	BoundingSphere sphere;

	void Init(std::vector<OpenGLInfo> gLInfos);

	void ConfigureBoundingSpheres(const tinyobj::attrib_t* attribs);

	int modelRefNum = 0;
	int diffuseRefNum = 1;
	int specRefNum = 2;
	float refCoef = 1;
	float rough = 0.2f;
	glm::mat4 worldTransform = glm::mat4(1);

	OpenGLInfo openGLInfo;

	void MoveTo(const glm::vec3 worldSpacePos);
	void MoveBy(const glm::vec3 amount);

	void RotateBy(const float amount, const glm::vec3 axis);
	void SetRotationToIdentity();

	void Draw(const int programID, float* sunDir);
};

