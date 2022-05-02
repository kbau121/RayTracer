#include "Image.h"

#include <cstdlib>
#include <algorithm>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image_write.h>

Image::Image(int w, int h, int c) {
	width = w;
	height = h;
	channels = c;

	buffer = (int*)malloc(width * height * channels * sizeof(int));
	final_buffer = (char*)malloc(width * height * channels);
}

void Image::setPixel(int x, int y, Color c) {
	int i = y * width * channels + x * channels;

	buffer[i]     = c.r;
	buffer[i + 1] = c.g;
	buffer[i + 2] = c.b;
}

int* Image::getBuffer() {
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

Color Image::getColor(int x, int y) {
	int i = y * width * channels + x * channels;
	return Color(buffer[i], buffer[i + 1], buffer[i + 2]);
}

Color Image::getColor(int i) {
	return Color(buffer[i], buffer[i + 1], buffer[i + 2]);
}

void Image::save(const char* filename) {
	// 1, 0.1f, 0.01f
	//ward_operator(0.1f);

	//10, 5, 1
	//reinhard_model(5);

	adaptive_logarithmic(10.f, 0.85f);

	stbi_write_png(filename, width, height, channels, final_buffer, width * channels);
}

float Image::px_illuminance(int x, int y) {
	Color col = getColor(x, y);

	return 0.27f * col.r + 0.67f * col.g + 0.06f * col.b;
}

float Image::px_illuminance(int i) {
	Color col = getColor(i);

	return 0.27f * col.r + 0.67f * col.g + 0.06f * col.b;
}

float Image::max_illuminance() {
	float max_I = 0.f;
	for (int x = 0; x < width; ++x) {
		for (int y = 0; y < height; ++y) {
			max_I = fmaxf(max_I, px_illuminance(x, y));
		}
	}

	return max_I;
}

float Image::log_avg_illuminance() {
	float sum = 0.f;
	for (int x = 0; x < width; ++x) {
		for (int y = 0; y < height; ++y) {
			sum += logf(0.0000001f + px_illuminance(x, y));
		}
	}

	return expf(sum / (width * height));
}

void Image::ward_operator(float Ldmax) {
	float Lwa = log_avg_illuminance();
	float sf = powf((1.219f + powf(Ldmax / 2.f, 0.4f)) / (1.219f + powf(Lwa, 0.4f)), 2.5f);

	for (int i = 0; i < width * height * channels; ++i) {
		int in_col = buffer[i];
		float out_col = sf * in_col / Ldmax;
		final_buffer[i] = (char) fmaxf(fminf(out_col, 255.f), 0.f);
	}
}

void Image::reinhard_model(float Ldmax) {
	float Lwa = log_avg_illuminance();
	float a = 0.18f;

	for (int i = 0; i < width * height * channels; ++i) {
		float Cs = a * px_illuminance(i) / Lwa;
		float Cr = Cs / (1 + Cs);
		int in_col = buffer[i];
		float out_col = in_col * Cr / Ldmax;

		//float in_col = buffer[i];
		//float out_col = 255.f * in_col / (255.f + in_col);

		final_buffer[i] = (char)fmaxf(fminf(out_col, 255.f), 0.f);
	}
}

void Image::adaptive_logarithmic(float Ldmax, float b) {
	float Lwa = log_avg_illuminance();

	float Lwmax = 0.f;
	for (int x = 0; x < width; ++x) {
		for (int y = 0; y < height; ++y) {
			Lwmax = fmaxf(px_illuminance(x, y), Lwmax);
		}
	}
	Lwmax /= Lwa;

	for (int i = 0; i < width * height * channels; ++i) {
		float Lw = px_illuminance(i) / Lwa;
		float Ld = (1.f / log10f(Lwmax + 1.f)) * logf(Lw + 1.f) / logf(2.f + 8.f * powf(Lw / Lwmax, logf(b) / logf(0.5f)));

		int in_col = buffer[i];
		float out_col = in_col * Ld / Ldmax;

		final_buffer[i] = (char)fmaxf(fminf(out_col, 255.f), 0.f);
	}
}