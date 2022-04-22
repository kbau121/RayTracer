#pragma once

#include "Primitive.h"

struct Sphere_Data
{
	glm::vec3 center;
	float radius;
};

class Sphere : public Primitive
{
public:
	static Intersection intersect(glm::vec3 v, glm::vec3 v0, Sphere_Data sphere);
	static Sphere_Data transform(glm::mat4x4 m, Sphere_Data sphere);
};