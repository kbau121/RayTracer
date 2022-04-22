#pragma once

#include "Intersection.h"

class BSDF
{
public:
	virtual Color illuminate(Intersection intersection, LightSource light);
};

