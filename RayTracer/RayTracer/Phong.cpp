#include "Phong.h"

Phong::Phong() {
	ka = 0.2f;
	kd = 0.4f;
	ks = 0.4f;
	ke = 20.f;
	diffuse = Color(255, 0, 0);
	specular = Color(255, 255, 255);
}

Phong::Phong(float _ka, float _kd, float _ks, float _ke, float _kr, float _kt, float _IOR, float _i_density, Color _diffuse, Color _specular) {
	ka =_ka;
	kd = _kd;
	ks = _ks;
	ke = _ke;
	kr = _kr;
	kt = _kt;
	IOR = _IOR;
	i_density = _i_density;
	diffuse = _diffuse;
	specular = _specular;
}

Color Phong::illuminate(Intersection intersection, LightSource light) {
	Color ambientComponent = diffuse;

	Color diffuseComponent = Color(0, 0, 0);
	Color specularComponent = Color(0, 0, 0);

	glm::vec3 S = glm::normalize(light.position - intersection.point);
	glm::vec3 N = intersection.normal;
	glm::vec3 R = -glm::reflect(S, N);
	glm::vec3 V = -intersection.incoming;

	float SN = glm::dot(S, N);
	if (SN > 0)
		diffuseComponent = diffuseComponent + diffuse * SN;

	float RV = glm::dot(R, V);
	if (RV > 0)
		specularComponent = specularComponent + specular * powf(RV, ke);

	//return ambientComponent * ka + diffuseComponent * kd + specularComponent * ks;
	return diffuseComponent * kd + specularComponent * ks;
}