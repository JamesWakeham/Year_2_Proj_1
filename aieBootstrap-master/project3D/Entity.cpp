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

void Entity::Init(std::vector<OpenGLInfo> gLInfos)
{
	openGLInfo = gLInfos[modelRefNum];
}

void Entity::ConfigureBoundingSpheres(const tinyobj::attrib_t* attribs)
{
	std::vector<vec3> tempVecs;
	for (int i = 0; i < attribs->vertices.size(); i+=3)
	{
		vec3 temp = vec3(attribs->vertices[i], attribs->vertices[i + 1], attribs->vertices[i+2]);
		tempVecs.push_back(temp);
	}
	sphere.fit(tempVecs);
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

	worldTransform = worldTransform * rotation;
}

void Entity::SetRotationToIdentity()
{
	worldTransform[0] = vec4(1, 0, 0, 0);
	worldTransform[1] = vec4(0, 1, 0, 0);
	worldTransform[2] = vec4(0, 0, 1, 0);
}

void Entity::Draw(const int programID, float* sunDir)
{	
	int loc;
	// set texture slot 
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, diffuseRefNum);

	// tell the shader where it is
	loc = glGetUniformLocation(programID, "diffuse");
	glUniform1i(loc, 0);

	// set texture slot 
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, specRefNum);

	// tell the shader where it is
	loc = glGetUniformLocation(programID, "specMap");
	glUniform1i(loc, 1);

	int roughness = glGetUniformLocation(programID, "roughness");
	glUniform1f(roughness, rough);
	// 0 = chrome, 1 = wood

	int reflectionCoefficient = glGetUniformLocation(programID, "reflectionCoefficient");
	glUniform1f(reflectionCoefficient, refCoef);
	// between 0 and 1

	int lightDir = glGetUniformLocation(programID, "LightDir");
	glUniform3fv(lightDir, 1, sunDir);


	loc = glGetUniformLocation(programID, "worldPositionMatrix");
	glUniformMatrix4fv(loc, 1, GL_FALSE, &(worldTransform[0][0]));

	glBindVertexArray(openGLInfo.m_VAO);
	glDrawArrays(GL_TRIANGLES, 0, openGLInfo.m_faceCount * 3);
}
