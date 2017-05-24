#pragma once

#include <glew.h>
#include <iostream>

/**
*	Represents the slider to go through the animation frame by frame.
*/
class Slider
{
public:
	Slider();
	void drawSlider(int windowWidth, int windowHeight, int frame, int frameCount, int id);
	int clickSlider(int x, int y);
	int dragSlider(int x);
	~Slider();
private:
	int backgroundLeft, backgroundRight, backgroundTop, backgroundBottom, numOfFrames;
	float pixelsPerFrame;
};

