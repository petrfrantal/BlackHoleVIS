#include "Slider.h"

Slider::Slider() {}

void Slider::drawSlider(int windowWidth, int windowHeight, int frame, int frameCount, int id) {

	numOfFrames = frameCount;
	int sliderWidth = 10;
	backgroundLeft = windowWidth / 10;
	backgroundRight = windowWidth - backgroundLeft;
	backgroundTop = windowHeight - 90 - id*70;
	backgroundBottom = windowHeight - 50 - id * 70;

	float backgroundLeft2 = 2 * float(backgroundLeft) / windowWidth - 1;
	float backgroundRight2 = 2 * float(backgroundRight) / windowWidth - 1;
	float backgroundTop2 = -(2 * float(backgroundTop) / windowHeight - 1);
	float backgroundBottom2 = -(2 * float(backgroundBottom) / windowHeight - 1);

	//float sliderStartPixel = (float)endPositionPixel;
	//sliderStartPixel = (float)endPositionPixel - startPositionPixel;
	//sliderStartPixel = (float)endPositionPixel - startPositionPixel - sliderWidth;
	//sliderStartPixel = (float)endPositionPixel - startPositionPixel - sliderWidth / frameCount;
	pixelsPerFrame = ((float)backgroundRight - backgroundLeft) / (frameCount - 1);
	float sliderPositionPixel = pixelsPerFrame * frame + backgroundLeft;


	float sliderStart = 2 * ((float)sliderPositionPixel - (float)sliderWidth / 2) / windowWidth - 1;
	float sliderdEnd = 2 * ((float)sliderPositionPixel + (float)sliderWidth / 2) / windowWidth - 1;

	glBegin(GL_QUADS);              // Each set of 4 vertices form a quad
	glColor3f(0.0f, 0.0f, 1.0f); // Blue
	glVertex2f(sliderStart, backgroundBottom2);    // x, y
	glVertex2f(sliderdEnd, backgroundBottom2);

	glVertex2f(sliderdEnd, backgroundTop2);
	glVertex2f(sliderStart, backgroundTop2);

	glColor3f(0.0f, 0.0f, 0.3f); //DarkBlue
	glVertex2f(backgroundLeft2, backgroundBottom2);    // x, y
	glVertex2f(backgroundRight2, backgroundBottom2);
	glVertex2f(backgroundRight2, backgroundTop2);
	glVertex2f(backgroundLeft2, backgroundTop2);

	glEnd();
}

int Slider::clickSlider(int x, int y) {
	if (x<backgroundLeft || x > backgroundRight || y<backgroundTop || y>backgroundBottom) {
		return -1;
	}
	int frame = roundf(((float)x - backgroundLeft) / pixelsPerFrame);
	return frame;
}

int Slider::dragSlider(int x) {
	if (x<backgroundLeft) {
		return 0;
	}
	else if (x > backgroundRight) {
		return numOfFrames - 1;
	}
	else {
		int frame = roundf(((float)x - backgroundLeft) / pixelsPerFrame);
		return frame;
	}
}

Slider::~Slider()
{
}
