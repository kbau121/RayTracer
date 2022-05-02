#pragma once

#include "Color.h"

class Image
{
public:
	Image(int w, int h, int c);

	void setPixel(int x, int y, Color c);

	int* getBuffer();
	int getWidth();
	int getHeight();
	int getChannels();
	Color getColor(int x, int y);

	void save(const char* filename);

private:
	int* buffer;
	char* final_buffer;
	int width;
	int height;
	int channels;

	Color getColor(int i);

	float px_illuminance(int x, int y);
	float px_illuminance(int i);
	float max_illuminance();
	float log_avg_illuminance();

	void ward_operator(float Ldmax);
	void reinhard_model(float Ldmax);
	void adaptive_logarithmic(float Ldmax, float b);
};