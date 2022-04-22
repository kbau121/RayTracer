#pragma once

class Image
{
public:
	Image();

	void setPixel(int x, int y);

	int* getBuffer();
	int getWidth();
	int getHeight();
	int getChannels();

	void save(char* filename);

private:
	int* buffer;
	int width;
	int height;
	int channels;
};