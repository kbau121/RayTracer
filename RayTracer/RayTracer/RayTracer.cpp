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

// Light data
const int light_count = 1;
LightSource* lights[light_count];

// Misc settings
const float error_tolerance = 0.01f;

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

int main() {

	Image img = Image(width, height, channels);

	Camera testCam = Camera(
		glm::vec3(38.49, -4.49, 2.38),
		glm::vec3(33.49, -4.49, 2.35),
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
	sph_arr[0] = Sphere_Data{ glm::vec3(27.54, -2.88, 2.27), 1.1f };
	sph_mats[0] = new Phong(0.2f, 0.4f, 0.4f, 75.f, Color(255, 0, 0), Color(255, 255, 255));

	sph_arr[1] = Sphere_Data{ glm::vec3(29.94, -4.73, 2.82), 1.1f };
	sph_mats[1] = new Phong(0.2f, 0.4f, 0.4f, 10.f, Color(0, 0, 255), Color(255, 255, 255));

	tri_arr[0] = Triangle_Data{ glm::vec3(32, 0.44, 8.6), glm::vec3(-32, 0.44, 8.6), glm::vec3(32, 0.44, -8.6) };
	tri_mats[0] = new CheckerBoard(glm::vec3(32.f, 0.44, 8.6), 1.f, 1.f, Color(187, 187, 61), Color(173, 1, 16));

	tri_arr[1] = Triangle_Data{ glm::vec3(-32, 0.44, -8.6), glm::vec3(32, 0.44, -8.6), glm::vec3(-32, 0.44, 8.6) };
	tri_mats[1] = new CheckerBoard(glm::vec3(32.f, 0.44, 8.6), 1.f, 1.f, Color(187, 187, 61), Color(173, 1, 16));

	// Apply world to camera transform
	for (int i = 0; i < sph_count; ++i) { sph_arr[i] = Sphere::transform(testCam.worldToCamera, sph_arr[i]);  }
	for (int i = 0; i < tri_count; ++i) { tri_arr[i] = Triangle::transform(testCam.worldToCamera, tri_arr[i]); }

	// Initialize light data
	lights[0] = new LightSource(glm::vec3(0, -100, 0), Color(255, 255, 255));

	for (int i = 0; i < light_count; ++i) { lights[i]->transform(testCam.worldToCamera); }

	// Main loop
	for (int x = 0; x < width; ++x) {
		for (int y = 0; y < height; ++y) {
			float t_x = (x - width / 2.f) / (max_dim / 2);
			float t_y = -(y - height / 2.f) / (max_dim / 2);

			glm::vec3 v = glm::normalize(glm::vec3(t_x, t_y, fov));

			/*
			for (int i = 0; i < prim_count; ++i) {
				Intersection tmp_w = primitives[i]->intersect(v);

				if (tmp_w.omega >= 0 && (tmp_w.omega < min_w.omega || min_w.omega < 0.f)) {
					prim = primitives[i];
					min_w = tmp_w;
				}
			}
			*/

			Intersection nearest = findIntersection(v);

			Color out_color = Color(0, 10, 40);

			if (nearest.type != NONE) {
				LightSource* illuminationLights[light_count];
				int illuminationLightCount = 0;

				for (LightSource* light : lights) {
					glm::vec3 shadowRay = glm::normalize(light->position - nearest.point);
					if (glm::dot(shadowRay, nearest.normal) <= 0) continue;

					Intersection shadowIntersection = findIntersection(shadowRay, nearest.point);

					if (shadowIntersection.omega < error_tolerance || shadowIntersection.omega == FLT_MAX) {
						illuminationLights[illuminationLightCount] = light;
						illuminationLightCount++;
					}
				}

				out_color = Color(0, 0, 0);
				for (int i = 0; i < illuminationLightCount; ++i) {
					switch (nearest.type)
					{
					case SPHERE:
						out_color = out_color + sph_mats[nearest.index]->illuminate(nearest, *illuminationLights[i]);
						break;
					case TRIANGLE:
						out_color = out_color + tri_mats[nearest.index]->illuminate(nearest, *illuminationLights[i]);
						break;
					default:
						break;
					}
				}
			}

			/*
			if (nearest.type != NONE) {

				LightSource* illuminationLights[light_count];
				int illuminationLightCount = 0;

				for (LightSource* light : lights) {
					glm::vec3 shadowRay = glm::normalize(light->position - min_w.point);

					bool illuminate = true;
					for (int i = 0; i < prim_count; ++i) {
						Intersection tmp_w = primitives[i]->intersect(shadowRay, min_w.point);

						if (tmp_w.omega > 0.01f) {
							illuminate = false;
							break;
						}
					}

					if (illuminate) {
						illuminationLights[illuminationLightCount] = light;
						illuminationLightCount++;
					}
				}

				min_w.lights = illuminationLights;
				min_w.lightCount = illuminationLightCount;
				out_color = prim->material->illuminate(min_w);
			}
			*/

			img.setPixel(x, y, out_color);
		}
	}

	img.save("test.png");

	return EXIT_SUCCESS;
}