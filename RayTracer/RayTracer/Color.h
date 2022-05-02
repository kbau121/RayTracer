#pragma once

#include <cstdint>

class Color
{
public:
	Color();
	Color(int n_r, int n_g, int n_b);

	Color operator* (float s);
	Color operator+ (Color c);
	Color operator* (Color c);

	int r;
	int g;
	int b;
};

