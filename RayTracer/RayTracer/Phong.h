#pragma once

#include "BSDF.h"

class Phong : public BSDF
{
public:
	Phong();
	Phong(float _ka, float _kd, float _ks, float _ke, float _kr, float _kt, float _IOR, float _i_density, Color _diffuse, Color _specular);

	virtual Color illuminate(Intersection intersection, LightSource light) override;

	float ka;
	float kd;
	float ks;
	float ke;
	Color diffuse;
	Color specular;
};

