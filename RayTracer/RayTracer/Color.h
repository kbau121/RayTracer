#pragma once

#include <cstdint>

class Color
{
public:
	Color();
	Color(uint8_t n_r, uint8_t n_g, uint8_t n_b);

	Color operator* (float s);
	Color operator+ (Color c);
	Color operator* (Color c);

	uint8_t r;
	uint8_t g;
	uint8_t b;
};

