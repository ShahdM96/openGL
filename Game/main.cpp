#define _USE_MATH_DEFINES // for C++
#include "InputManager.h"
// #include "../DisplayGLFW/display.h"
#include "game.h"
#include "../res/includes/glm/glm.hpp"
#include <fstream>
#include "stb_image.h"
#include <iostream>


void setIndex(unsigned char* image, int width, int hight, int x, int y, unsigned char val) {
	int index = 4 * (x * hight + y);
	if (index >= 4 * width * hight) return;
	image[index] = val;
	image[index + 1] = val;
	image[index + 2] = val;
	image[index + 3] = 255;
}


void floyd_St_Algorithm(unsigned char* image, int width, int hight) {
	int color1 = 256;//8-bit color
	int color2 = 16;//4-bit color
	float** newImage = (float**)malloc(sizeof(float*) * hight);
	for (int x = 0; x < hight; x++) {
		newImage[x] = (float*)malloc(sizeof(float) * width);
	}
	for (int x = 0; x < hight; x++) {
		for (int y = 0; y < width; y++) {
			newImage[x][y] = image[4 * (y + x * hight)];
		}
	}

	for (int x = 0; x < hight; x++) {
		for (int y = 0; y < width ; y++) {

			float value1 = newImage[x][y];//old value
			unsigned char value2;//new value
			if(int(value1 + 0.5) >= color1) {
				value2 = color2 - 1;
			}
			else {
				value2 = int(value1 + 0.5) / (color1 / color2);
			}
			value2 *= color1 / color2;
			newImage[x][y] = value2;
			float error = value1 - value2;
			if (x < hight && y + 1 < width) {
				newImage[x][y + 1] += error * 7.0 / 16;
			}
			if (x + 1 < hight) {
				if (y > 0) {
					newImage[x + 1][y - 1] += error * 3.0 / 16;
				}
				if (y < width) {
					newImage[x + 1][y] += error * 5.0 / 16;
				}
				if (y + 1 < width) {
					newImage[x + 1][y + 1] += error * 1.0 / 16;
				}
			}
		}
	}
	std::ofstream output;
	output.open("../hw1/img6.txt", std::ios::out | std::ios::trunc);
	output.clear();
	for (int x = 0; x < hight; x++) {
		for (int y = 0; y < width; y++) {
			setIndex(image, width, hight, x, y, (unsigned char)newImage[x][y]);
			output << image[4 * (y + x * hight)] / (color1 / color2);
			if (x < hight - 1 || y < width - 1) {
				output << ',';
			}
		}
		output << '\n';
	}
	output.close();
}

unsigned char* halftone_pattern(unsigned char* image, int width, int hight) {
	unsigned char* newImage = (unsigned char*)malloc(4 * sizeof(unsigned char) * width * hight * 4);
	for (int x = 0; x < hight; x++) {
		for (int y = 0; y < width * 4; y++) {
			float avg = (image[4 * (y + x * hight)])/ 255.0;
			x *= 2; y *= 2; width *= 2; hight *= 2;
			if (avg < 0.2) {
				setIndex(newImage, width, hight, x + 1, y, 0);
				setIndex(newImage, width, hight, x, y + 1, 0);
				setIndex(newImage, width, hight, x + 1, y + 1, 0);
				setIndex(newImage, width, hight, x, y, 0);
			}
			else if (avg < 0.4) {
				setIndex(newImage, width, hight, x, y, 0);
				setIndex(newImage, width, hight, x + 1, y, 255);
				setIndex(newImage, width, hight, x, y + 1, 0);
				setIndex(newImage, width, hight, x + 1, y + 1, 0);

			}
			else if (avg < 0.6) {
				setIndex(newImage, width, hight, x, y, 0);
				setIndex(newImage, width, hight, x + 1, y, 255);
				setIndex(newImage, width, hight, x, y + 1, 255);
				setIndex(newImage, width, hight, x + 1, y + 1, 0);
			}
			else if (avg < 0.8) {
				setIndex(newImage, width, hight, x, y, 0);
				setIndex(newImage, width, hight, x + 1, y, 255);
				setIndex(newImage, width, hight, x, y + 1, 255);
				setIndex(newImage, width, hight, x + 1, y + 1, 255);
			}
			else {
				setIndex(newImage, width, hight, x, y, 255);
				setIndex(newImage, width, hight, x + 1, y, 255);
				setIndex(newImage, width, hight, x, y + 1, 255);
				setIndex(newImage, width, hight, x + 1, y + 1, 255);
			}
			
			x = x/2; y =y/2; width =width/2; hight =hight/2;
		}

	}
	std::ofstream output;
	output.open("../hw1/img5.txt", std::ios::out | std::ios::trunc);
	output.clear();
	hight *= 2;
	width *= 2;
	for (int i = 0; i < hight; i++) {
		for (int j = 0; j < width; j++) {
			output << (255 == newImage[4 * (j + i * hight)]) ? '1' : '0';
			if (i < hight - 1 || j < width - 1) {
				output << ',';
			}
		}
		output << '\n';
	}
	output.close();
	

	return newImage;
}

void maximum_suppression(const unsigned char* newImage, const float* grad_direction, unsigned char* output) {
	const int width = 256;
	const int hight = 256;
	for (int x = 0; x < hight; ++x) {
		for (int y = 0; y < width; ++y) {

			if (x == 0 || y == 0 || x == hight - 1 || y == width - 1) {
				output[x * width + y] = 0;
				continue;
			}
			//gradient directions
			float angle = grad_direction[x * width + y];

			//neighboring pixels
			float gradient1, gradient2;

			if (angle < 0) {
				angle += M_PI;
			}

			if ((angle >= 0 && angle < M_PI / 8) || (angle >= 15 * M_PI / 8 && angle < 2 * M_PI) || (angle >= 7 * M_PI / 8 && angle < 9 * M_PI / 8)) {
				gradient1 = newImage[x * width + y + 1];
				gradient2 = newImage[x * width + y - 1];
			}
			else if ((angle >= M_PI / 8 && angle < 3 * M_PI / 8) || (angle >= 9 * M_PI / 8 && angle < 11 * M_PI / 8)) {
				gradient1 = newImage[(x + 1) * width + y + 1];
				gradient2 = newImage[(x - 1) * width + y - 1];
			}
			else if ((angle >= 3 * M_PI / 8 && angle < 5 * M_PI / 8) || (angle >= 11 * M_PI / 8 && angle < 13 * M_PI / 8)) {
				gradient1 = newImage[(x + 1) * width + y];
				gradient2 = newImage[(x - 1) * width + y];
			}
			else if ((angle >= 5 * M_PI / 8 && angle < 7 * M_PI / 8) || (angle >= 13 * M_PI / 8 && angle < 15 * M_PI / 8)) {
				gradient1 = newImage[(x - 1) * width + y + 1];
				gradient2 = newImage[(x + 1) * width + y - 1];
			}
			else {
				gradient1 = 0;
				gradient2 = 0;
			}

			//non-maximum
			if (newImage[x * width + y] >= gradient1 && newImage[x * width + y] >= gradient2) {
				if (newImage[x * width + y] < 28) output[x * width + y] = 0;
				else output[x * width + y] = 255;
			}
			else {
				output[x * width + y] = 0;
			}
		}
	}
}


void Canny_Edge_Detector(const unsigned char* image, unsigned char* output) {
	const int width = 256;
	const int hight = 256;
	unsigned char newImage[hight * width];
	for (int x = 0; x < hight * width * 4; x += 4) {
		newImage[x / 4] = image[x];
	}

	unsigned char convolution[width * hight];
	unsigned char derivative[width * hight];
	float gradient_direction[width * hight];
	unsigned char max_sup[hight * width];

	float gaussian_kernel[] = { 0.125,0.125,0.125,0.125,0.25, 0.125,0.0625,0.125,0.0625 };

	//convolution
	int count = 0;
	for (int x = 0; x < hight; ++x) {
		for (int y = 0; y < width; ++y) {
			int sum = 0;
			for (int k = 0; k < 3; ++k) { //row x
				for (int l = 0; l < 3; ++l) { //element xy
					int image_x = x + k - 1;
					int image_y = y + l - 1;
					if (image_x < 0) {
						image_x = 1;
					}
					if (image_y < 0) {
						image_y = 1;
					}
					if (image_x >= hight) {
						image_x = hight - 1;
					}
					if (image_y >= width) {
						image_y = width - 1;
					}
					sum += newImage[image_x * width + image_y] * gaussian_kernel[k * 3 + l];
				}
			}
			count++;
			convolution[x * (width) + y] = sum;
		}
	}

	//calculate derivative
	unsigned char grayscale_dx[width * hight];
	unsigned char grayscale_dy[width * hight];

	for (int x = 1; x < hight; ++x) {
		for (int y = 1; y < width; ++y) {
			grayscale_dx[x * width + y] = abs(convolution[x * width + y] - convolution[x * width + y - 1]);
			grayscale_dy[x * width + y] = abs(convolution[x * width + y] - convolution[(x - 1) * width + y]);
			gradient_direction[x * width + y] = atan2(grayscale_dy[x * width + y], grayscale_dx[x * width + y]);
		}
	}

	for (int x = 0; x < hight; x++) {
		for (int y = 0; y < width; ++y) {
			if (x == 0 || y == 0) {
				derivative[x * width + y] = 0;
			}
			derivative[x * width + y] = abs((int)std::round(sqrt(grayscale_dx[x * width + y] * grayscale_dx[x * width + y] + grayscale_dy[x * width + y] * grayscale_dy[x * width + y]))) % 256;
		}
	}

	// non-maximum suppression
	maximum_suppression(derivative, gradient_direction, max_sup);

	std::ofstream outfile;
	outfile.open("../hw1/img4.txt", std::ios::out | std::ios::trunc);
	outfile.clear();

	for (int x = 0; x < hight; x++) {
		for (int y = 0; y < width; y++) {
			outfile << (255 == max_sup[x * width + y]) ? '1' : '0';
			if (x < hight - 1 || y < width - 1)outfile << ',';
		}
		outfile << "\n";
	}
	outfile.close();

	for (int x = 0; x < hight * width; x++) {
		output[4 * x] = max_sup[x];
		output[4 * x + 1] = max_sup[x];
		output[4 * x + 2] = max_sup[x];
		output[4 * x + 3] = 255;
	}
}

//******************************************************************************

int main(int argc,char *argv[])
{
	const int DISPLAY_WIDTH = 512;
	const int DISPLAY_HEIGHT = 512;
	const float CAMERA_ANGLE = 0.0f;
	const float NEAR = 1.0f;
	const float FAR = 100.0f;

	Game *scn = new Game(CAMERA_ANGLE,(float)DISPLAY_WIDTH/DISPLAY_HEIGHT,NEAR,FAR);
	
	Display display(DISPLAY_WIDTH, DISPLAY_HEIGHT, "OpenGL");
	
	Init(display);
	
	scn->Init();

	display.SetScene(scn);

	// bottom left - halftone
	int width, height, numComponents;
	unsigned char* data = stbi_load("../res/textures/lena256.jpg", &width, &height, &numComponents, 4);
	scn->AddTexture(width * 2, height * 2, halftone_pattern(data, width, height));
	scn->SetShapeTex(0, 0);
	scn->CustomDraw(1, 0, scn->BACK, true, false, 0);

	// bottom right - Floyd-Steinberg
	unsigned char* data_FS = stbi_load("../res/textures/lena256.jpg", &width, &height, &numComponents, 4);
	floyd_St_Algorithm(data_FS, width, height);
	scn->AddTexture(256, 256, data_FS);
	scn->SetShapeTex(0, 1);
	scn->CustomDraw(1, 0, scn->BACK, false, false, 1);


	// top left - original
	scn->AddTexture("../res/textures/lena256.jpg", false);
	scn->SetShapeTex(0, 2);
	scn->CustomDraw(1, 0, scn->BACK, false, true, 2);


	// top right - Canny Edge Detector
	unsigned char* data_canny = stbi_load("../res/textures/lena256.jpg", &width, &height, &numComponents, 4);
	unsigned char output[256 * 256 * 4];
	Canny_Edge_Detector(data_canny, output);
	scn->AddTexture(width, height, output);
	scn->SetShapeTex(0, 3);
	scn->CustomDraw(1, 0, scn->BACK, false, true, 3);


	scn->Motion();
	display.SwapBuffers();

	while(!display.CloseWindow())
	{
		//scn->Draw(1,0,scn->BACK,true,false);
		//scn->Motion();
		//display.SwapBuffers();
		display.PollEvents();	
			
	}
	delete scn;
	return 0;
}
