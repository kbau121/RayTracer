#include "Color.h"
#include <math.h>

Color::Color() {
	r = 0;
	g = 0;
	b = 0;
}

Color::Color(int n_r, int n_g, int n_b) {
	r = n_r;
	g = n_g;
	b = n_b;
}

Color Color::operator*(float s) {
	return Color((int)(r * s), (int)(g * s), (int)(b * s));
}

Color Color::operator+(Color c) {
	return Color(r + c.r, g + c.g, b + c.b);
}

Color Color::operator*(Color c) {
	return Color(r * c.r / 255, g * c.g / 255, b * c.b / 255);
}