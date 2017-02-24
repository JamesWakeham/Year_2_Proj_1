#pragma once
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "Application.h"

using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::mat4;

class Camera
{
	// camera pos and rot in world space
	mat4 worldTransform;
	// inverse of world transform;
	mat4 viewTransform;
	// near and far planes 
	mat4 projectionTransform;
	// view * projection
	mat4 projectionViewTransform;

public:
	Camera();
	~Camera();

	vec4 worldPos;

	Camera(float windowHeight, float windowLength);
	void Start();
	void Start(float windowHeight, float windowWidth);

	virtual void Update(float deltaTime) = 0;
	void SetPerspective(float fieldOfView, float aspectRatio, float near, float far);
	void SetLookAt(vec3 from, vec3 to, vec3 up);
	void SetPos(vec3 pos);
	vec3 GetPos();
	void SetWorldTransform(mat4 transform);
	mat4 GetWorldTransform();
	mat4 GetView();
	mat4 GetProjection();
	mat4 GetProjectionView();
	vec3 GetRow(int row);
protected:
	void UpdateProjectionViewMatrix();
};

