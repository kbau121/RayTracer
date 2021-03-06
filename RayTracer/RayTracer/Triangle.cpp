#include "Triangle.h"
#include "Phong.h"

Intersection Triangle::intersect(glm::vec3 v, glm::vec3 v0, Triangle_Data triangle) {
	glm::vec3 e1 = triangle.p1 - triangle.p0;
	glm::vec3 e2 = triangle.p2 - triangle.p0;
	glm::vec3 T = v0 - triangle.p0;
	glm::vec3 P = glm::cross(v, e2);
	glm::vec3 Q = glm::cross(T, e1);
	float Pe1 = glm::dot(P, e1);

	if (Pe1 == 0) {
		return Intersection();
	}

	glm::vec3 out = glm::vec3(glm::dot(Q, e2), glm::dot(P, T), glm::dot(Q, v)) / Pe1;

	if (out.y < 0 || out.z < 0 || out.y + out.z > 1) {
		return Intersection();
	}

	float omega = out.x;
	glm::vec3 point = omega * v + v0;

	return Intersection(omega, point, glm::normalize(glm::cross(e1, e2)), v, TRIANGLE, -1);
}

Triangle_Data Triangle::transform(glm::mat4x4 m, Triangle_Data triangle) {
	glm::vec4 p0_4 = glm::vec4(triangle.p0, 1.f);
	glm::vec4 p1_4 = glm::vec4(triangle.p1, 1.f);
	glm::vec4 p2_4 = glm::vec4(triangle.p2, 1.f);

	p0_4 = m * p0_4;
	p1_4 = m * p1_4;
	p2_4 = m * p2_4;

	triangle.p0 = glm::vec3(p0_4) / p0_4.w;
	triangle.p1 = glm::vec3(p1_4) / p1_4.w;
	triangle.p2 = glm::vec3(p2_4) / p2_4.w;

	return triangle;
}