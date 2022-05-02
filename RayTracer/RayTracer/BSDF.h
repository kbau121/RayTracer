#pragma once

#include "Intersection.h"

class BSDF
{
public:
	virtual Color illuminate(Intersection intersection, LightSource light);

public:
	float kr = 0;
	float kt = 0;
	float IOR = 1.f;
	float i_density = 0;
};

