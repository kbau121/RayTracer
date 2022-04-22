#pragma once

#include "Color.h"
#include <glm/glm.hpp>

class LightSource
{
public:
	LightSource(glm::vec3 _position, Color _color);

	void transform(glm::mat4x4 m);

	glm::vec3 position;
	Color color;
};

