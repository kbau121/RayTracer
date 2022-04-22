#pragma once

#include "LightSource.h"
#include "PrimTypes.h"
#include <glm/glm.hpp>

class Intersection
{
public:
	Intersection();
	Intersection(float _omega, glm::vec3 _point, glm::vec3 _normal, glm::vec3 _incoming, PrimType type, int index);

	float omega;
	glm::vec3 point;
	glm::vec3 normal;
	glm::vec3 incoming;
	glm::vec3 reflective;

	PrimType type;
	int index;
};