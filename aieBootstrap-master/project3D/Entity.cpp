#include "Entity.h"



Entity::Entity()
{
}


Entity::~Entity()
{
}

Entity::Entity(const vec3 startPos)
{
	MoveTo(startPos);
}

void Entity::MoveTo(const vec3 worldSpacePos)
{
	worldTransform[3] = vec4(worldSpacePos, 1);
}

void Entity::MoveBy(const vec3 amount)
{
	MoveTo(worldTransform[3].xyz + amount);
}

void Entity::RotateBy(const float amount, const vec3 axis)
{
	mat4 rotation;

	rotation = glm::rotate(rotation, amount, axis);

	worldTransform = rotation * worldTransform;
}

void Entity::SetRotationToIdentity()
{
	worldTransform[0] = vec4(1, 0, 0, 0);
	worldTransform[1] = vec4(0, 1, 0, 0);
	worldTransform[2] = vec4(0, 0, 1, 0);
}
