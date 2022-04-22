#include "Intersection.h"

Intersection::Intersection() {
	omega = FLT_MAX;
	point = glm::vec3(0);
	normal = glm::vec3(0);
	incoming = glm::vec3(0);
	reflective = glm::vec3(0);
	type = NONE;
	index = -1;
}

Intersection::Intersection(float _omega, glm::vec3 _point, glm::vec3 _normal, glm::vec3 _incoming, PrimType _type, int _index) {
	omega = _omega;
	point = _point;
	normal = _normal;
	incoming = _incoming;
	reflective = glm::reflect(_incoming, _normal);
	type = _type;
	index = _index;
}