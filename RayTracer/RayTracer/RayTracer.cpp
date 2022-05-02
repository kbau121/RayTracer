#define _USE_MATH_DEFINES

#include "RayTracer.h"
#include "Image.h"
#include <cstdlib>
#include <algorithm>
#include <iostream>
#include <glm/glm.hpp>
#include "happly.h"
#include "BRDFRead.h"
#include <math.h>

#include "Camera.h"

#include "Sphere.h"
#include "Triangle.h"

#include "Phong.h"
#include "CheckerBoard.h"

using namespace std;

// Object data
int tri_count = 2;
const int sph_count = 4;

Triangle_Data * tri_arr;
Sphere_Data sph_arr[sph_count];

// Material data
BSDF ** tri_mats;
BSDF* sph_mats[sph_count];
double* _brdf;
BRDFRead reader = BRDFRead();

// Camera data
const int width = 1280, height = 720, max_dim = std::max(width, height), channels = 3;
const float fov = 1.25f;
//const int width = 128, height = 128, max_dim = std::max(width, height), channels = 3;
//const float fov = 1.25f;

// Light data
const int light_count = 1;
LightSource* lights[light_count];

// Misc settings
const float error_tolerance = 0.01f;
const int max_bounce = 10;

glm::vec3 rectToSph(glm::vec3 v) {
	return glm::vec3(1, atanf(v.y / v.x), acos(v.z));
}

glm::vec3 randDirection() {
	glm::vec3 random = glm::normalize(glm::vec3(rand(), rand(), rand())) - 0.5f;
	return glm::normalize(random);
}

Intersection findIntersection(glm::vec3 v, glm::vec3 v0 = glm::vec3(0)) {
	Intersection nearest = Intersection();

	for (int i = 0; i < sph_count; ++i) {
		Intersection tmp_intersection = Sphere::intersect(v, v0, sph_arr[i]);
		if (nearest.omega > tmp_intersection.omega && tmp_intersection.omega > error_tolerance) { nearest = tmp_intersection; nearest.index = i; }
	}

	for (int i = 0; i < tri_count; ++i) {
		Intersection tmp_intersection = Triangle::intersect(v, v0, tri_arr[i]);
		if (nearest.omega > tmp_intersection.omega && tmp_intersection.omega > error_tolerance) { nearest = tmp_intersection; nearest.index = i; }
	}

	return nearest;
}

Color illuminate(glm::vec3 v, glm::vec3 v0, int bounce_depth, bool volume = false) {
	Intersection nearest = findIntersection(v, v0);

	Color out_color = Color(0, 10, 40) * 50;

	if (nearest.type != NONE) {
		LightSource* illuminationLights[light_count];
		int illuminationLightCount = 0;

		for (LightSource* light : lights) {
			glm::vec3 shadowRay = glm::normalize(light->position - nearest.point);
			if (glm::dot(shadowRay, nearest.normal) <= 0) continue;

			Intersection shadowIntersection = findIntersection(shadowRay, nearest.point);

			//if (shadowIntersection.omega < error_tolerance || shadowIntersection.omega == FLT_MAX) {
			if (shadowIntersection.omega == FLT_MAX) {
				illuminationLights[illuminationLightCount] = light;
				illuminationLightCount++;
			}
		}

		float kr = 0;
		float kt = 0;
		float IOR = 1.f;
		float i_density = 0.f;
		double* brdf = nullptr;
		switch (nearest.type)
		{
		case SPHERE:
			if (sph_mats[nearest.index]) {
				kr = sph_mats[nearest.index]->kr;
				kt = sph_mats[nearest.index]->kt;
				IOR = sph_mats[nearest.index]->IOR;
				i_density = sph_mats[nearest.index]->i_density;
			}
			else {
				brdf = _brdf;
			}
			break;
		case TRIANGLE:
			if (tri_mats[nearest.index]) {
				kr = tri_mats[nearest.index]->kr;
				kt = tri_mats[nearest.index]->kt;
				IOR = tri_mats[nearest.index]->IOR;
				i_density = tri_mats[nearest.index]->i_density;
			}
			else {
				brdf = _brdf;
			}
			break;
		}

		out_color = Color(0, 0, 0);
		for (int i = 0; i < illuminationLightCount; ++i) {
			switch (nearest.type)
			{
			case SPHERE:
				if (sph_mats[nearest.index])
					out_color = out_color + sph_mats[nearest.index]->illuminate(nearest, *illuminationLights[i]) * illuminationLights[i]->color;
				break;
			case TRIANGLE:
				if (tri_mats[nearest.index])
					out_color = out_color + tri_mats[nearest.index]->illuminate(nearest, *illuminationLights[i]) * illuminationLights[i]->color;
				break;
			default:
				break;
			}
		}

		if (bounce_depth < max_bounce) {
			if (!brdf) {
				if (kr > 0) {
					out_color = out_color + illuminate(nearest.reflective, nearest.point, bounce_depth + 1) * kr;
				}

				if (kt > 0) {
					glm::vec3 n;
					glm::vec3 t;

					if (glm::dot(nearest.normal, v) > 0.f)
					{
						n = -nearest.normal;
					}
					else {
						n = nearest.normal;
						IOR = 1.f / IOR;
					}

					float cosI = -glm::dot(n, v);
					float sinT2 = IOR * IOR * (1.f - cosI * cosI);

					if (sinT2 > 1.f)
					{
						t = nearest.reflective;
					}
					else {
						float cosT = sqrtf(1.f - sinT2);
						t = IOR * v + (IOR * cosI - cosT) * n;
					}

					if (i_density > 0) {
						if (volume) {
							float dist2 = powf(glm::length(v0 - nearest.point), 2);
							out_color = out_color + illuminate(t, nearest.point, bounce_depth + 1, true) + Color(255, 255, 255) * dist2;
						}
						else
						{
							out_color = out_color + illuminate(t, nearest.point, bounce_depth + 1, true);
						}
					}
					else {
						out_color = out_color + illuminate(t, nearest.point, bounce_depth + 1) * kt;
					}
				}
			}
			else {
				glm::vec3 incoming = rectToSph(v);
				glm::vec3 p_outgoing = rectToSph(nearest.reflective);

				glm::vec3 summation = glm::vec3(0);
				int p_res = 1;
				int t_res = 1;
				for (float p = 0; p <= M_PI_2 - M_PI_2 / p_res; p += M_PI_2 / p_res) {
					for (float t = -M_PI; t <= M_PI - M_PI * 2 / t_res; t += M_PI * 2 / t_res) {
						glm::vec3 out_vector = nearest.reflective;
						glm::vec3 outgoing = rectToSph(out_vector);

						double red, green, blue;
						reader.lookup_brdf_val(brdf, incoming.y, incoming.z, outgoing.y, outgoing.z, red, green, blue);

						float vn = fmaxf(glm::dot(out_vector, nearest.normal), 0.f);
						Color in_color = illuminate(out_vector, nearest.point, bounce_depth + 1);

						summation = summation + glm::vec3(in_color.r * vn / red, in_color.g * vn / green, in_color.b * vn / blue) * 0.005f;
					}
				}

				summation *= 1 / (p_res * t_res);
				out_color = out_color + Color(summation.x, summation.y, summation.z);
			}
		}
	}

	return out_color;
}

void genPly(const char* filename) {
	happly::PLYData plyIn(filename);

	std::vector<std::array<double, 3>> vPos = plyIn.getVertexPositions();
	std::vector<std::vector<size_t>> fInd = plyIn.getFaceIndices<size_t>();

	size_t tmp_size = fInd.size();

	Triangle_Data* tmp_arr = (Triangle_Data*)realloc(tri_arr, (tri_count + tmp_size) * sizeof(Triangle_Data));
	BSDF ** tmp_mats = (BSDF**)realloc(tri_mats, (tri_count + tmp_size) * sizeof(BSDF*));

	if (tmp_arr && tmp_mats) {
		tri_arr = tmp_arr;
		tri_mats = tmp_mats;
	} else return;

	Phong* default_mat = new Phong(0.f, 1.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, Color(125, 125, 125), Color(0, 0, 0));

	for (unsigned int i = 0; i < tmp_size; ++i) {

		std::array <double, 3> v0 = vPos[fInd[i][0]];
		std::array <double, 3> v1 = vPos[fInd[i][1]];
		std::array <double, 3> v2 = vPos[fInd[i][2]];

		glm::vec3 p0 = glm::vec3(v0[0], v0[1], v0[2]);
		glm::vec3 p1 = glm::vec3(v1[0], v1[1], v1[2]);
		glm::vec3 p2 = glm::vec3(v2[0], v2[1], v2[2]);

		tri_arr[tri_count + i] = Triangle_Data{ p0, p1, p2 };
		tri_mats[tri_count + i] = default_mat;
	}
	tri_count += tmp_size;
}

int main() {
	reader.read_brdf("brdfs/gold-metallic-paint.binary", _brdf);

	glm::vec3 sph_in = rectToSph(glm::normalize(glm::vec3(1, 1, 1)));
	glm::vec3 sph_out = rectToSph(glm::normalize(glm::vec3(1, 1, 1)));
	double red, green, blue;
	reader.lookup_brdf_val(_brdf, sph_in.y, sph_in.z, sph_out.y, sph_out.z, red, green, blue);
	cout << red << ", " << green << ", " << blue << endl;

	tri_arr = (Triangle_Data*)malloc(tri_count * sizeof(Triangle_Data));
	tri_mats = (BSDF**)malloc(tri_count * sizeof(BSDF*));

	//genPly("bunny/reconstruction/bun_zipper_res4.ply");

	Image img = Image(width, height, channels);

	Camera testCam = Camera(
		//glm::vec3(38.49, -4.49, 2.38),
		glm::vec3(33.49, -1.5, 2.38),
		//glm::vec3(0, 0.f, -0.2f),

		//glm::vec3(33.49, -4.49, 2.35),
		glm::vec3(28.49, -1, 2.35),
		//glm::vec3(0, 0.1f, 0),

		glm::vec3(0, -1, 0)
	);

	/*
	{
		A* a = new A(1, 2, "3");
		std::unique_ptr<A> a = std::make_unique<A>(1, 2, "3");
		std::shared_ptr<A> a = std::make_shared<A>(1, 2, "3");

		a->b();
		A& a2 = *a;
		A* a3 = a.get();

		void test(std::unique_ptr<A> a) {
		}
		test(a);  // IDK, won't compile? Might move a into test() and your a is nullptr now?
		test(std::move(a));  // Your a is definitely nullptr now. Still might not compile!

		void test2(std::unique_ptr<A>& a) {
		}
		test2(a);  // Everything is great!

		void test3(A& a) {
			a.b();
			// NOT THIS: a->b();
		}
		test3(*a);  // Also acceptable if you don't want std::unique_ptr<...> in function signature.


	}
	*/

	// Initialize object data
	//sph_arr[0] = Sphere_Data{ glm::vec3(27.54, -2.88, 2.27), 1.1f };
	sph_arr[0] = Sphere_Data{ glm::vec3(25, -0.75, 3.5f), 1.1f };
	sph_mats[0] = new Phong(0.1f, 0.f, 0.6f, 75.f, 0.f, 1.f, 0.8f, 0.f, Color(255, 255, 255), Color(255, 255, 255));

	//sph_arr[1] = Sphere_Data{ glm::vec3(29.94, -4.73, 2.82), 1.1f };
	sph_arr[1] = Sphere_Data{ glm::vec3(25, -1, 1), 1.1f };
	sph_mats[1] = new Phong(0.1f, 0.f, 0.6f, 10.f, 1.f, 0.0f, 0.0f, 0.f, Color(255, 255, 255), Color(255, 255, 255));

	sph_arr[2] = Sphere_Data{ glm::vec3(25, -1, 5.9f), 1.1f };
	sph_mats[2] = new Phong(0.f, 0.f, 0.f, 0.f, 0.f, 1.f, 1.f, 0.1f, Color(255, 255, 255), Color(255, 255, 255));

	sph_arr[3] = Sphere_Data{ glm::vec3(25, -1, -1.5f), 1.1f };
	sph_mats[3] = nullptr;

	tri_arr[0] = Triangle_Data{ glm::vec3(32, 0.44, 8.6), glm::vec3(-32, 0.44, 8.6), glm::vec3(32, 0.44, -8.6) };
	tri_mats[0] = (BSDF*)new CheckerBoard(glm::vec3(32.f, 0.44, 8.6), 1.f, 1.f, Color(187, 187, 61), Color(173, 1, 16));

	tri_arr[1] = Triangle_Data{ glm::vec3(-32, 0.44, -8.6), glm::vec3(32, 0.44, -8.6), glm::vec3(-32, 0.44, 8.6) };
	tri_mats[1] = (BSDF*)new CheckerBoard(glm::vec3(32.f, 0.44, 8.6), 1.f, 1.f, Color(187, 187, 61), Color(173, 1, 16));

	// Apply world to camera transform
	for (int i = 0; i < sph_count; ++i) { sph_arr[i] = Sphere::transform(testCam.worldToCamera, sph_arr[i]);   }
	for (int i = 0; i < tri_count; ++i) { tri_arr[i] = Triangle::transform(testCam.worldToCamera, tri_arr[i]); }

	// Initialize light data
	lights[0] = new LightSource(glm::vec3(0, -100, 0), Color(255, 255, 255) * 20.f);

	for (int i = 0; i < light_count; ++i) { lights[i]->transform(testCam.worldToCamera); }

	// Main loop
	for (int x = 0; x < width; ++x) {
		cout << ((float)x) / (width) << endl;
		for (int y = 0; y < height; ++y) {
			float t_x = (x - width / 2.f) / (max_dim / 2);
			float t_y = -(y - height / 2.f) / (max_dim / 2);

			glm::vec3 v = glm::normalize(glm::vec3(t_x, t_y, fov));

			Color out_color = illuminate(v, glm::vec3(0), 0);

			img.setPixel(x, y, out_color);
		}
	}

	img.save("test.png");

	return EXIT_SUCCESS;
}