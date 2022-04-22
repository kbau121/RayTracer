#include "Color.h"

Color::Color() {
	r = (uint8_t) 0;
	g = (uint8_t) 0;
	b = (uint8_t) 0;
}

Color::Color(uint8_t n_r, uint8_t n_g, uint8_t n_b) {
	r = n_r;
	g = n_g;
	b = n_b;
}

Color Color::operator*(float s) {
	return Color((uint8_t)(r * s), (uint8_t)(g * s), (uint8_t)(b * s));
}

Color Color::operator+(Color c) {
	return Color(r + c.r, g + c.g, b + c.b);
}