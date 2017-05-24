#include <SDL.h>
#include "Slider.h"
#undef main
#include <glm\glm.hpp>
#include <iostream>
#include "Window.h"
#include "Camera.h"
#include "ArrowShader.h"
#include "LineShader.h"
#include "Mesh.h"
#include <math.h>
#include "Visualization.h"
#include <string>

int main(int argc, char * argv[]) {

	std::string filePath = "data/el2_32_32_32b.csv";
	if (argc == 2) {
		filePath = argv[1];
	}

	// Window defitions
	const int windowWidth = 1280;
	const int windowHeight = 720;
	std::string windowName = "Black Holes Visualization";
	Window window(windowName, windowWidth, windowHeight);

	// Camera definitions
	glm::vec3 cameraPosition = glm::vec3(0.0f, 0.0f, -50.0f);
	float cameraHorizontalAngle = -90.0f;
	float cameraVerticalAngle = 0.0f;
	Camera camera(cameraPosition, cameraHorizontalAngle, cameraVerticalAngle, 70.0f, (float)windowWidth / windowHeight, 0.1f, 10000.0f);

	// Shader definitions
	ArrowShader arrowShader("./Shaders/ArrowShader");
	LineShader lineShader("./Shaders/LineShader");

	Visualization vis(&arrowShader, &lineShader);
	vis.loadArray(filePath);

	// create a frame slider
	Slider samplingParameterSlider;
	Slider cuttingPlaneSlider;

	// variables related to free camera
	bool leftMouseButtonPressed = false;		// buttons
	bool rightMouseButtonPressed = false;
	bool mouseWheelPressed = false;
	bool leftMouseButtonSlider0 = false;
	bool leftMouseButtonSlider1 = false;
	float horizontalAngleDelta = 0.0f;			// angles to rotate the camera
	float verticalAngleDelta = 0.0f;
	float forwardBackwardMovementDelta = 0.0f;	// variables to move the camera
	float horizontalTranslationDelta = 0.0f;
	float verticalTranslationDelta = 0.0f;
	int mouseXPos;								// auxiliary variables
	int mouseYPos;
	int newMouseXPos;
	int newMouseYPos;
	int samplingParameterSliderClickValue = -1;
	int cuttingPlaneSliderClickValue = -1;
	int num0 = 12;
	int num1 = 5;

	// SDL window loop
	SDL_Event e;
	bool appRunning = true;
	while (appRunning) {
		while (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT) {
				appRunning = false;
			} else if (e.type == SDL_KEYDOWN) {
				if (e.key.keysym.sym == SDLK_ESCAPE) {
					appRunning = false;
				} else if (e.key.keysym.sym == SDLK_SPACE) {
					vis.setSamplingParameter(num0 + 3);
				} else if (e.key.keysym.sym == SDLK_x) {
					vis.setCuttingPlane(xPlane, (double)num1 / 50);
				} else if (e.key.keysym.sym == SDLK_y) {
					vis.setCuttingPlane(yPlane, (double)num1 / 50);
				} else if (e.key.keysym.sym == SDLK_z) {
					vis.setCuttingPlane(zPlane, (double)num1 / 50);
				} else if (e.key.keysym.sym == SDLK_c) {
					vis.endCuttingPlane();
				} else if (e.key.keysym.sym == SDLK_t) {
					vis.changeModel();
				}
			} else if (e.type == SDL_MOUSEBUTTONDOWN) {
				if (e.button.button == SDL_BUTTON_LEFT) {
					samplingParameterSliderClickValue = samplingParameterSlider.clickSlider(e.motion.x, e.motion.y);
					cuttingPlaneSliderClickValue = cuttingPlaneSlider.clickSlider(e.motion.x, e.motion.y);
					if (samplingParameterSliderClickValue >= 0) {
						num0 = samplingParameterSliderClickValue;
						leftMouseButtonSlider0 = true;
					} else if (cuttingPlaneSliderClickValue >= 0) {
						num1 = cuttingPlaneSliderClickValue;
						leftMouseButtonSlider1 = true;
					} else {
						leftMouseButtonPressed = true;
					}
				} else if (e.button.button == SDL_BUTTON_RIGHT) {
					rightMouseButtonPressed = true;
				} else if (e.button.button == SDL_BUTTON_MIDDLE) {
					mouseWheelPressed = true;
				}
				mouseXPos = e.motion.x;
				mouseYPos = e.motion.y;
			} else if (e.type == SDL_MOUSEBUTTONUP) {
				leftMouseButtonPressed = false;
				rightMouseButtonPressed = false;
				mouseWheelPressed = false;
				leftMouseButtonSlider0 = false;
				leftMouseButtonSlider1 = false;
			} else if (e.type == SDL_MOUSEMOTION) {
				if (leftMouseButtonPressed) {
					newMouseXPos = e.motion.x;				// rotate the camera
					newMouseYPos = e.motion.y;
					horizontalAngleDelta = 0.005f * (newMouseXPos - mouseXPos);
					verticalAngleDelta = 0.005f * (newMouseYPos - mouseYPos);
					camera.updateCameraAngles(horizontalAngleDelta, verticalAngleDelta);
					mouseXPos = newMouseXPos;
					mouseYPos = newMouseYPos;
				} else if (rightMouseButtonPressed) {
					newMouseXPos = e.motion.x;				// move the camera forward of backward (in the camera direction)
					newMouseYPos = e.motion.y;
					forwardBackwardMovementDelta = newMouseXPos - mouseXPos;
					camera.moveCameraForwardBackward(forwardBackwardMovementDelta);
					mouseXPos = newMouseXPos;
					mouseYPos = newMouseYPos;
				} else if (mouseWheelPressed) {
					newMouseXPos = e.motion.x;				// translate the camera left, right, up or down
					newMouseYPos = e.motion.y;
					horizontalTranslationDelta = newMouseXPos - mouseXPos;
					verticalTranslationDelta = newMouseYPos - mouseYPos;
					camera.moveCameraRightLeftUpDown(horizontalTranslationDelta, verticalTranslationDelta);
					mouseXPos = newMouseXPos;
					mouseYPos = newMouseYPos;
				} else if (leftMouseButtonSlider0) {
					samplingParameterSliderClickValue = samplingParameterSlider.dragSlider(e.motion.x);
					num0 = samplingParameterSliderClickValue;
					samplingParameterSliderClickValue = -1;
				} else if (leftMouseButtonSlider1) {
					cuttingPlaneSliderClickValue = cuttingPlaneSlider.dragSlider(e.motion.x);
					num1 = cuttingPlaneSliderClickValue;
					cuttingPlaneSliderClickValue = -1;
				}
			}
		}
		window.clear(0.0f, 0.0f, 0.0f, 1.0f);
		vis.draw(camera);
		samplingParameterSlider.drawSlider(windowWidth, windowHeight, num0, 48, 0);
		cuttingPlaneSlider.drawSlider(windowWidth, windowHeight, num1, 51, 1);
		window.swapBuffers();
	}

	return 0;
}