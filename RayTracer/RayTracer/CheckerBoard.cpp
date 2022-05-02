#include "CheckerBoard.h"

CheckerBoard::CheckerBoard() {
	origin = glm::vec3(0.f);
	col_size = 1.f;
	row_size = 1.f;
	c1 = Color(0, 0, 0);
	c2 = Color(255, 255, 255);
}

CheckerBoard::CheckerBoard(glm::vec3 _origin, float _col_size, float _row_size, Color _c1, Color _c2) {
	origin = _origin;
	col_size = _col_size;
	row_size = _row_size;
	c1 = _c1;
	c2 = _c2;
}

Color CheckerBoard::illuminate(Intersection intersection, LightSource light) {
	glm::vec3 tmp_start = intersection.point - origin;
	glm::vec2 start = glm::vec2(tmp_start.x, tmp_start.z);

	Color diffuse = Color(0, 0, 0);

	Color col = c1;

	int dr = abs((int)(start.x / row_size)) % 2; if (!dr) dr = -1;
	int dc = abs((int)(start.y / col_size)) % 2; if (!dc) dc = -1;

	if (dr * dc == 1) {
		col = c2;
	}

	glm::vec3 S = glm::normalize(light.position - intersection.point);
	glm::vec3 N = intersection.normal;

	float SN = glm::dot(S, N);
	if (SN > 0)
		diffuse = diffuse + col * SN;

	return diffuse;
	//return Color((int)(intersection.point.x * 255), (int)(intersection.point.y * 255), (int)(intersection.point.z * 255));
}