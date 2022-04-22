#pragma once

#include "Primitive.h"

struct Triangle_Data
{
	glm::vec3 p0;
	glm::vec3 p1;
	glm::vec3 p2;
};

class Triangle : public Primitive
{
public:
	static Intersection intersect(glm::vec3 v, glm::vec3 v0, Triangle_Data triangle);
	static Triangle_Data transform(glm::mat4x4 m, Triangle_Data triangle);
};

