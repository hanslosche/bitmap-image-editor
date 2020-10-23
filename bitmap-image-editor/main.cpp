#include <iostream>
#include "image.hpp"

int main() {
	std::cout << "Hello, BMP!" << std::endl;

	Image img;
	img.read("in.bmp");
	img.write("out.bmp");
	img.clear();

	return 0;
}