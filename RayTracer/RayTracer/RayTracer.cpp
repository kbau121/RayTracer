#include "RayTracer.h"
#include "Image.h"
#include <cstdlib>
#include <algorithm>
#include <iostream>
#include <glm/glm.hpp>

#include "Camera.h"

#include "Sphere.h"
#include "Triangle.h"

#include "Phong.h"
#include "CheckerBoard.h"

using namespace std;

// Object data
const int tri_count = 2;
const int sph_count = 2;

Triangle_Data tri_arr[tri_count];
Sphere_Data sph_arr[sph_count];

// Material data
BSDF* tri_mats[tri_count];
BSDF* sph_mats[sph_count];

// Camera data
const int width = 1280, height = 720, max_dim = std::max(width, height), channels = 3;
const float fov = 1.25f;
//const int width = 128, height = 128, max_dim = std::max(width, height), channels = 3;
//const float fov = 13.5f;

// Light data
const int light_count = 1;
LightSource* lights[light_count];

// Misc settings
const float error_tolerance = 0.01f;
const int max_bounce = 3;

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

Color illuminate(glm::vec3 v, glm::vec3 v0, int bounce_depth) {
	Intersection nearest = findIntersection(v, v0);

	Color out_color = Color(0, 10, 40);

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
		switch (nearest.type)
		{
		case SPHERE:
			kr = sph_mats[nearest.index]->kr;
			kt = sph_mats[nearest.index]->kt;
			IOR = sph_mats[nearest.index]->IOR;
			break;
		case TRIANGLE:
			kr = tri_mats[nearest.index]->kr;
			kt = tri_mats[nearest.index]->kt;
			IOR = tri_mats[nearest.index]->IOR;
			break;
		}

		out_color = Color(0, 0, 0);
		for (int i = 0; i < illuminationLightCount; ++i) {
			switch (nearest.type)
			{
			case SPHERE:
				out_color = out_color + sph_mats[nearest.index]->illuminate(nearest, *illuminationLights[i]) * illuminationLights[i]->color;
				break;
			case TRIANGLE:
				out_color = out_color + tri_mats[nearest.index]->illuminate(nearest, *illuminationLights[i]) * illuminationLights[i]->color;
				break;
			default:
				break;
			}
		}

		if (bounce_depth < max_bounce) {
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

				out_color = out_color + illuminate(t, nearest.point, bounce_depth + 1) * kt;
			}
		}
	}

	return out_color;
}

int main() {

	Image img = Image(width, height, channels);

	Camera testCam = Camera(
		//glm::vec3(38.49, -4.49, 2.38),
		glm::vec3(33.49, -1.5, 2.38),
		//glm::vec3(33.49, -4.49, 2.35),
		glm::vec3(28.49, -1, 2.35),
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
	sph_mats[0] = new Phong(0.1f, 0.f, 0.6f, 75.f, 0.f, 1.f, 0.8f, Color(255, 255, 255), Color(255, 255, 255));

	//sph_arr[1] = Sphere_Data{ glm::vec3(29.94, -4.73, 2.82), 1.1f };
	sph_arr[1] = Sphere_Data{ glm::vec3(25, -1, 1), 1.1f };
	sph_mats[1] = new Phong(0.1f, 0.f, 0.6f, 10.f, 1.f, 0.0f, 0.0f, Color(255, 255, 255), Color(255, 255, 255));

	tri_arr[0] = Triangle_Data{ glm::vec3(32, 0.44, 8.6), glm::vec3(-32, 0.44, 8.6), glm::vec3(32, 0.44, -8.6) };
	tri_mats[0] = new CheckerBoard(glm::vec3(32.f, 0.44, 8.6), 1.f, 1.f, Color(187, 187, 61), Color(173, 1, 16));

	tri_arr[1] = Triangle_Data{ glm::vec3(-32, 0.44, -8.6), glm::vec3(32, 0.44, -8.6), glm::vec3(-32, 0.44, 8.6) };
	tri_mats[1] = new CheckerBoard(glm::vec3(32.f, 0.44, 8.6), 1.f, 1.f, Color(187, 187, 61), Color(173, 1, 16));

	// Apply world to camera transform
	for (int i = 0; i < sph_count; ++i) { sph_arr[i] = Sphere::transform(testCam.worldToCamera, sph_arr[i]);  }
	for (int i = 0; i < tri_count; ++i) { tri_arr[i] = Triangle::transform(testCam.worldToCamera, tri_arr[i]); }

	// Initialize light data
	lights[0] = new LightSource(glm::vec3(0, -100, 0), Color(255, 255, 255) * 1.f);

	for (int i = 0; i < light_count; ++i) { lights[i]->transform(testCam.worldToCamera); }

	// Main loop
	for (int x = 0; x < width; ++x) {
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