#include "Sphere.h"
#include "Phong.h"

Intersection Sphere::intersect(glm::vec3 v, glm::vec3 v0, Sphere_Data sphere)
{
	float B = 2 * (v.x * (v0.x - sphere.center.x) + v.y * (v0.y - sphere.center.y) + v.z * (v0.z - sphere.center.z));
	float C = powf(v0.x - sphere.center.x, 2) + powf(v0.y - sphere.center.y, 2) + powf(v0.z - sphere.center.z, 2) - powf(sphere.radius, 2);

	float t = powf(B, 2) - 4 * C;

	if (t < 0) return Intersection();

	float w1 = (-B + sqrtf(t)) / 2;
	float w2 = (-B - sqrtf(t)) / 2;

	float omega;

	if (w1 < 0.f) omega = w2;
	else if (w2 < 0.f) omega = w1;
	else omega = fminf(w1, w2);

	glm::vec3 point = omega * v + v0;
	return Intersection(omega, point, glm::normalize(point - sphere.center), v, SPHERE, -1);
}

Sphere_Data Sphere::transform(glm::mat4x4 m, Sphere_Data sphere) {
	glm::vec4 center_4 = glm::vec4(sphere.center, 1.f);

	center_4 = m * center_4;

	sphere.center = glm::vec3(center_4) / center_4.w;

	return sphere;
}