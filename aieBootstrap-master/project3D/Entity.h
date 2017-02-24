#pragma once
#include "Application3D.h"

using glm::vec3;
using glm::vec4;
using glm::mat4;

class Entity
{
	
public:
	OpenGLInfo openGLInfo;
	mat4 worldTransform = mat4(1);

	Entity();
	~Entity();
	Entity(const vec3 startPos);

	void MoveTo(const vec3 worldSpacePos);
	void MoveBy(const vec3 amount);

	void RotateBy(const float amount, const vec3 axis);
	void SetRotationToIdentity();

	void Draw();
};

