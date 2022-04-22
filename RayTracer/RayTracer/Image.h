#pragma once

#include "Color.h"

class Image
{
public:
	Image(int w, int h, int c);

	void setPixel(int x, int y, Color c);

	unsigned char* getBuffer();
	int getWidth();
	int getHeight();
	int getChannels();

	void save(const char* filename);

private:
	unsigned char* buffer;
	int width;
	int height;
	int channels;
};