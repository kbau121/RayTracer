#include "LightSource.h"

LightSource::LightSource(glm::vec3 _position, Color _color) {
	position = _position;
	color = _color;
}

void LightSource::transform(glm::mat4x4 m) {
	glm::vec4 position_4 = glm::vec4(position, 1);

	position_4 = m * position_4;

	position = position_4 / position_4.w;
}