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

	/*
	scn->AddTexture("../res/textures/snake1.png", false);// top right
	scn->AddShader("../res/shaders/basicShader");
	scn->SetShapeTex(0, 3);
	scn->CustomDraw(1, 0, scn->BACK, false, true, 3);
	*/
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
