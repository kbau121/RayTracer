#pragma once

#include <glm/glm.hpp>

class Camera
{
public:
	Camera();
	Camera(glm::vec3 _eyepoint, glm::vec3 _lookat, glm::vec3 up);

	glm::vec3 toCamera(glm::vec3 v);
	glm::vec3 toCamera(glm::vec4 v);

	glm::mat4 worldToCamera;

private:
	glm::mat4 genWorldToCamera();

	glm::vec3 eyepoint;
	glm::vec3 lookat;
	glm::vec3 up;
};

