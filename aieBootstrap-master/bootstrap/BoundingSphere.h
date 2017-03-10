#pragma once
#include <glm\glm.hpp>
#include <vector>

class BoundingSphere
{
public:
	BoundingSphere() : centre(0), radius(0){}
	~BoundingSphere();

	void fit(const std::vector<glm::vec3>& points);

	glm::vec3 centre; 
	float radius;
};

