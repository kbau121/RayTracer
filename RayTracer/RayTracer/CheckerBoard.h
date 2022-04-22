#pragma once

#include "BSDF.h"

class CheckerBoard : public BSDF
{
public:
	CheckerBoard();
	CheckerBoard(glm::vec3 _origin, float _col_size, float _row_size, Color _c1, Color _c2);

	virtual Color illuminate(Intersection intersection, LightSource light) override;

	glm::vec3 origin;
	float col_size;
	float row_size;
	Color c1;
	Color c2;
};

