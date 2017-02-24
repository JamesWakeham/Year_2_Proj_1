#pragma once
#include "Camera.h"
#include "Input.h"

class FlyCam : public Camera
{
	float speed, lookSpeed;
	vec2 mousePosLastUpdate;
	vec3 up;
public:
	FlyCam();
	~FlyCam();
	FlyCam(float windowHeight, float windowLength);

	void MouseLookUpdate(float deltaTime);
	void FlyCamStart();
	virtual void Update(float deltaTime);
	void SetSpeed(float speed);
};

