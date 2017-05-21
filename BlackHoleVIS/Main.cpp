#include <SDL.h>
#undef main
#include <glm\glm.hpp>
#include <iostream>
#include "Window.h"
#include "Camera.h"
#include "ArrowShader.h"
#include "LineShader.h"
#include "Mesh.h"

int main(int argc, char * argv[]) {
	// Window defitions
	const int windowWidth = 1280;
	const int windowHeight = 720;
	std::string windowName = "Black Holes Visualization";
	Window window(windowName, windowWidth, windowHeight);

	// Camera definitions
	glm::vec3 cameraPosition = glm::vec3(0.0f, 0.0f, 50.0f);
	float cameraHorizontalAngle = 90.0f;
	float cameraVerticalAngle = 0.0f;
	Camera camera(cameraPosition, cameraHorizontalAngle, cameraVerticalAngle, 70.0f, (float)windowWidth / windowHeight, 0.1f, 10000.0f);

	// Shader definitions
	ArrowShader arrowShader("./Shaders/ArrowShader");
	LineShader lineShader("./Shaders/LineShader");

	// Mesh definitions
	ArrowMesh arrowMesh(&arrowShader);

	// variables related to free camera
	bool leftMouseButtonPressed = false;		// buttons
	bool rightMouseButtonPressed = false;
	bool mouseWheelPressed = false;
	float horizontalAngleDelta = 0.0f;			// angles to rotate the camera
	float verticalAngleDelta = 0.0f;
	float forwardBackwardMovementDelta = 0.0f;	// variables to move the camera
	float horizontalTranslationDelta = 0.0f;
	float verticalTranslationDelta = 0.0f;
	int mouseXPos;								// auxiliary variables
	int mouseYPos;
	int newMouseXPos;
	int newMouseYPos;

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
				}
			} else if (e.type == SDL_MOUSEBUTTONDOWN) {
				if (e.button.button == SDL_BUTTON_LEFT) {
					leftMouseButtonPressed = true;
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
				}
			}
		}
		window.clear(0.0f, 0.0f, 0.0f, 1.0f);
		// here will be the draws (we have to compute the transform matrices and so on - from the vector data)
		glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		glm::mat4 translate = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -450.0));
		glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(50.0f, 50.0f, 50.0f));
		glm::mat4 modelMatrix = translate * scale * rotation;
		glm::vec3 materialDiffuse = glm::vec3(1.0f, 1.0f, 1.0f);
		float vectorLength = 1.0f;
		glm::vec3 redMat = glm::vec3(1.0f, 0.2f, 0.2f); 
		glm::vec3 blueMat = glm::vec3(0.7f, 0.7f, 0.7f);
		materialDiffuse = vectorLength * redMat + (1 - vectorLength) * blueMat;
		//arrowMesh.draw(&arrowShader, camera, modelMatrix, materialDiffuse);

		// zaklad pro kostku dat
		int dimension = 512;
		int samplingX = 10;
		int samplingY = 5;
		int samplingZ = 5;
		for (int i = 0; i < samplingX; i++) {
			float translationX = i * 512 / (float)samplingX;
			for (int j = 0; j < samplingY; j++) {
				float translationY = j * 512 / (float)samplingY;
				for (int k = 0; k < samplingZ; k++) {
					float translationZ = k * 512 / (float)samplingZ;
					translate = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, translationZ - 450.0f));
					scale = glm::scale(glm::mat4(1.0f), glm::vec3(3.0f, 3.0f, 3.0f));
					modelMatrix = translate * scale * rotation;
					arrowMesh.draw(&arrowShader, camera, modelMatrix, materialDiffuse);
				}
			}
		}

		window.swapBuffers();
	}

	return 0;
}