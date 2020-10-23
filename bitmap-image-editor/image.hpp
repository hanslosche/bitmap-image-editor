#ifndef IMAGE_HPP
#define IMAGE_HPP

#include "bmp.h"

#include <stdio.h>
#include <stdlib.h>

class Image {
public:
	void generate(LONG _width, LONG _height) {
		width = _width;
		height = _height;
		
		// alocate memory for arrays
		data = new RGBTRIPLE * [height];
		for (int i = 0; i < height; i++) {
			data[i] = new RGBTRIPLE[width];
		}
	}

	void clear() {
		for (int i = 0; i < height; i++) {
			data[i] = nullptr;
		}
		data = nullptr;

		// rest properties
		width = 0;
		height = 0;
	}

	bool inBounds(int x, int y) {
		return x < width&& y < height;
	}

	bool setPixel(int x, int y, BYTE r, BYTE g, BYTE b) {
		if (!inBounds(x, y)) {
			return false;
		}
		data[y][x] = { b, g, r };
		return true;
	}

	bool setPixel(int x, int y, double r, double g, double b) {
		return setPixel(x, y,
			r * 255 + 0.5,
			g * 255 + 0.5,
			b * 255 + 0.5);
	}

	bool read(const char* file) {
		// open file
		FILE* inptr;
		errno_t error = fopen_s(&inptr, file, "r");
		if (error != 0) {
			return false;
		}

		// read header and set properties
		BITMAPFILEHEADER bf;
		fread(&bf, sizeof(BITMAPFILEHEADER), 1, inptr);

		BITMAPINFOHEADER bi;
		fread(&bi, sizeof(BITMAPINFOHEADER), 1, inptr);

		width = bi.biWidth;
		height = bi.biHeight;

		//calculate  padding so line length becomes multiple of 4
		int padding = 4 - (width * sizeof(RGBTRIPLE)) % 4;

		int rowSize = width * sizeof(RGBTRIPLE) + padding;

		BYTE* row = new BYTE[rowSize];
		data = new RGBTRIPLE * [height];
		for (int i = 0; i < height; i++) {
			data[i] = new RGBTRIPLE[width];
			fread(row, sizeof(BYTE), rowSize, inptr);
			for (int j = 0; j < width; j++) {
				data[i][j] = {
					row[j * 3],
					row[j * 3 + 1],
					row[j * 3 + 2]
				};
			}
		}
		fclose(inptr);
		return true;
	}

	bool write(const char* file) {
		// open file
		FILE* outptr;
		errno_t error = fopen_s(&outptr, file, "w");
		if (error != 0) {
			return false;
		}

		int padding = 4 - (width * sizeof(RGBTRIPLE)) % 4;
		DWORD imgSize = height * (width * sizeof(RGBTRIPLE) + padding);

		// set/write headers
		BITMAPFILEHEADER bf;
		bf.bfType = 0x4D42;
		bf.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + imgSize;
		bf.bfReserved1 = 0;
		bf.bfReserved2 = 0;
		bf.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
		fwrite(&bf, sizeof(BITMAPFILEHEADER), 1, outptr);

		BITMAPINFOHEADER bi;
		bi.biSize = sizeof(BITMAPINFOHEADER);
		bi.biWidth = width;
		bi.biHeight = height;
		bi.biPlanes = 1;
		bi.biBitCount = 24;
		bi.biCompression = 0;
		bi.biSizeImage = 0;
		bi.biXPelsPerMeter = 0;
		bi.biYPelsPerMeter = 0;
		bi.biClrUsed = 0;
		bi.biClrImportant = 0;
		fwrite(&bi, sizeof(BITMAPINFOHEADER), 1, outptr);

		for (int i = 0; i < height; i++) {
			// write row
			fwrite(data[i], sizeof(RGBTRIPLE), width, outptr);

			// write padding
			if (padding > 0) {
				for (int j = 0; j < padding; j++) {
					fputc(0x00, outptr);
				}
			}
		}

		fclose(outptr);
		return true;
	}



private:
	LONG width;
	LONG height;

	RGBTRIPLE** data;
};

#endif
