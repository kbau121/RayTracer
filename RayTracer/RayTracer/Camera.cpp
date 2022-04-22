#include "Camera.h"

Camera::Camera() {
	eyepoint = glm::vec3(0, 0, 0);
	lookat = glm::vec3(0, 0, -1);
	up = glm::vec3(0, 1, 0);

	worldToCamera = genWorldToCamera();
}

Camera::Camera(glm::vec3 _eyepoint, glm::vec3 _lookat, glm::vec3 _up) {
	eyepoint = _eyepoint;
	lookat = _lookat;
	up = _up;

	worldToCamera = genWorldToCamera();
}

glm::vec3 Camera::toCamera(glm::vec3 v) {
	glm::vec4 tmp = worldToCamera * glm::vec4(v, 1);

	return glm::vec3(tmp.x / tmp.w, tmp.y / tmp.w, tmp.z / tmp.w);
}

glm::vec3 Camera::toCamera(glm::vec4 v) {
	glm::vec4 tmp = worldToCamera * v;

	return glm::vec3(tmp.x / tmp.w, tmp.y / tmp.w, tmp.z / tmp.w);
}

glm::mat4 Camera::genWorldToCamera() {
	//glm::vec3 n = glm::normalize(eyepoint - lookat);
	glm::vec3 n = glm::normalize(lookat - eyepoint);
	glm::vec3 u = glm::normalize(glm::cross(up, n));
	glm::vec3 v = glm::cross(n, u);

	return glm::mat4{
		u.x, v.x, n.x, 0,
		u.y, v.y, n.y, 0,
		u.z, v.z, n.z, 0,
		-glm::dot(eyepoint, u), -glm::dot(eyepoint, v), -glm::dot(eyepoint, n), 1
	};
}