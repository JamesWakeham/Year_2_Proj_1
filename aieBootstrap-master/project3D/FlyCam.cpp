#include "FlyCam.h"



FlyCam::FlyCam()
{
	FlyCamStart();
	Camera::Start();
}


FlyCam::~FlyCam()
{

}

FlyCam::FlyCam(float windowHeight, float windowLength)
{
	FlyCamStart();
	Camera::Start(windowHeight, windowLength);
}

void FlyCam::FlyCamStart()
{
	speed = 5;
	lookSpeed = 0.1f;
	aie::Input* input = aie::Input::getInstance();
}


void FlyCam::Update(float deltaTime)
{
	aie::Input* input = aie::Input::getInstance();

	vec3 direction;
	float speedMultiplier =1;
	if (input->isKeyDown(aie::INPUT_KEY_W)) {
		direction = direction + GetWorldTransform()[2].xyz *-1;
	}
	else if (input->isKeyDown(aie::INPUT_KEY_S)) {
		direction = direction + GetWorldTransform()[2].xyz;
	}

	if (input->isKeyDown(aie::INPUT_KEY_A)) {
		direction = direction + GetWorldTransform()[0].xyz *-1;
	}
	else if (input->isKeyDown(aie::INPUT_KEY_D)) {
		direction = direction + GetWorldTransform()[0].xyz;
	}

	if (input->isKeyDown(aie::INPUT_KEY_LEFT_SHIFT)) {
		speedMultiplier = 3;
	}
	else if (input->isKeyDown(aie::INPUT_KEY_LEFT_CONTROL)) {
		speedMultiplier = 0.5;
	}

	direction = direction *deltaTime * speed * speedMultiplier;
	SetPos(vec3(GetWorldTransform()[3].xyz + direction));

	MouseLookUpdate(deltaTime);
}

void FlyCam::MouseLookUpdate(float deltaTime)
{
	aie::Input* input = aie::Input::getInstance();
	if (input->wasKeyPressed(aie::INPUT_KEY_LEFT_ALT)) { lockRot = !lockRot; }
	if (lockRot) {
		vec2 mousePosThisUpdate = vec2(input->getMouseX(), input->getMouseY());
		if (mousePosLastUpdate == vec2(0, 0)) {
			mousePosLastUpdate = mousePosThisUpdate;
		}

		mat4 currentTransform = GetWorldTransform();
		currentTransform[3] = vec4(0, 0, 0, 1);
		mat4 rotation;

		rotation = glm::rotate(rotation, (mousePosLastUpdate.x - mousePosThisUpdate.x)*deltaTime*lookSpeed, vec3(0, 1, 0));
		rotation *= glm::rotate((mousePosLastUpdate.y - mousePosThisUpdate.y)*deltaTime*-lookSpeed, GetRow(0));

		currentTransform = rotation * currentTransform;
		currentTransform[3] = GetWorldTransform()[3];
		SetWorldTransform(currentTransform);

		mousePosLastUpdate = mousePosThisUpdate;
	}
	else {
		mousePosLastUpdate = vec2(input->getMouseX(), input->getMouseY());
	}
}



void FlyCam::SetSpeed(float speed)
{
}
