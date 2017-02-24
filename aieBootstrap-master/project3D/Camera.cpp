#include "Camera.h"

Camera::Camera()
{
	Start(720, 1280);
}

Camera::~Camera()
{
}

Camera::Camera(float windowHeight, float windowWidth)
{
	Start(windowHeight, windowWidth);
}

void Camera::Start()
{
	Start(720, 1280);
}

void Camera::Start(float windowHeight, float windowWidth)
{
	SetPos(vec3(10));
	SetLookAt(vec3(10), vec3(0), vec3(0, 1, 0));
	//viewTransform = glm::lookAt(vec3(10), vec3(0), vec3(0, 1, 0));
	projectionTransform = glm::perspective(glm::pi<float>() * 0.25f,
		windowWidth / windowHeight,
		0.1f, 1000.f);
	UpdateProjectionViewMatrix();
}

void Camera::SetPerspective(float fieldOfView, float aspectRatio, float near, float)
{

}

void Camera::SetLookAt(vec3 from, vec3 to, vec3 up)
{
	viewTransform = glm::lookAt(from, to, up);
	worldTransform = glm::inverse(viewTransform);
}

void Camera::SetPos(vec3 pos)
{
	worldTransform[3] = vec4(pos, 1);
	viewTransform = glm::inverse(worldTransform);
	UpdateProjectionViewMatrix();
	worldPos = worldTransform[3];
}

vec3 Camera::GetPos()
{
	return vec3(worldTransform[3]);
}

void Camera::SetWorldTransform(mat4 transform)
{
	worldTransform = transform;
	viewTransform = glm::inverse(worldTransform);
	UpdateProjectionViewMatrix();
}

mat4 Camera::GetWorldTransform()
{
	return worldTransform;
}

mat4 Camera::GetView()
{
	return viewTransform;
}

mat4 Camera::GetProjection()
{
	return projectionTransform;
}

mat4 Camera::GetProjectionView()
{
	return projectionViewTransform;
}

vec3 Camera::GetRow(int row)
{
	return worldTransform[row].xyz;
}

void Camera::UpdateProjectionViewMatrix()
{
	projectionViewTransform = projectionTransform * viewTransform;
}
