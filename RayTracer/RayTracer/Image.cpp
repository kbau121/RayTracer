#include "Image.h"

#include <cstdlib>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image_write.h>

Image::Image(int w, int h, int c) {
	width = w;
	height = h;
	channels = c;

	buffer = (unsigned char*) malloc(width * height * channels);
}

void Image::setPixel(int x, int y, Color c) {
	int i = y * width * channels + x * channels;

	buffer[i]     = c.r;
	buffer[i + 1] = c.g;
	buffer[i + 2] = c.b;
}

unsigned char* Image::getBuffer() {
	return buffer;
}

int Image::getWidth() {
	return width;
}

int Image::getHeight() {
	return height;
}

int Image::getChannels() {
	return channels;
}

void Image::save(const char* filename) {
	stbi_write_png(filename, width, height, channels, buffer, width * channels);
}