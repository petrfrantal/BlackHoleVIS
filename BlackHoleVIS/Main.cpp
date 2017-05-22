#include <SDL.h>
#undef main
#include <glm\glm.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <iostream>
#include "Window.h"
#include "Camera.h"
#include "ArrowShader.h"
#include "LineShader.h"
#include "Mesh.h"
#include <math.h>
#include "Visualization.h"

// returns angle in radians, vertical angle, has to be in the same plane
float angleBetweenVectorsVertical(glm::vec3 first, glm::vec3 second) {
	float dot = glm::dot(first, second);
	float det = first.x * second.y - second.x * first.y;
	float angle = glm::degrees(atan2(det, dot));
	if (angle != 0.0f) {
		angle = -angle;
	}
	if (angle > 90.0f && angle <= 180.0f) {
		angle = 180 - angle;
	} else if (angle < -90.0f && angle > -180.0f) {
		angle = -180 - angle;
	}
	return glm::radians(angle);
}

float angleBetweenVectorsHorizontal(glm::vec3 first, glm::vec3 second) {
	float dot = glm::dot(first, second);
	float det = first.x * second.z - second.x * first.z;
	float angle = glm::degrees(atan2(det, dot));
	if (angle != 0.0f) {
		angle = -angle;
	}
	return glm::radians(angle);
}

float computeVerticalAngle(glm::vec3 v) {
	if (v.x == 0.0f && v.y == 0.0f) {
		return 0.0f;
	}
	glm::vec3 vXY = glm::vec3(v.x, v.y, 0.0f);
	if (vXY.x == 0.0f && v.z != 0.0f) {   // if the vectors turns into same as y axis, we would get wrong result, so we rotate it
		glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glm::vec4 vRotated = rotation * glm::vec4(v, 1.0f);
		vXY = glm::vec3(vRotated.x, vRotated.y, 0.0f);
	}
	vXY = glm::normalize(vXY);
	glm::vec3 xAxis = glm::vec3(1.0f, 0.0f, 0.0f);
	return angleBetweenVectorsVertical(vXY, xAxis);
}

float computeHorizontalAngle(glm::vec3 v) {
	if (v.x == 0.0f && v.z == 0.0f) {
		return 0.0f;
	}
	glm::vec3 vXZ = glm::vec3(v.x, 0.0f, v.z);
	vXZ = glm::normalize(vXZ);
	glm::vec3 xAxis = glm::vec3(1.0f, 0.0f, 0.0f);
	return angleBetweenVectorsHorizontal(vXZ, xAxis);
}

int main(int argc, char * argv[]) {

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

	Visualization vis(&arrowShader);
	vis.loadArray("data/el2_32_32_32b.csv");

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
		glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1.0f, 1.0f, 0.0f));
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
		modelMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		//glm::mat4 rotationAroundY = glm::rotate(glm::mat4(1.0f), glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		modelMatrix = glm::rotate(modelMatrix, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		modelMatrix = glm::rotate(modelMatrix, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		//rotation = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		//modelMatrix = rotationAroundY * modelMatrix;

		int dimension = 512;
		int samplingX = 10;
		int samplingY = 5;
		int samplingZ = 5;
		glm::mat4 usedModelMatrix;
		for (int i = 0; i < samplingX; i++) {
			float translationX = i * 512 / (float)samplingX;
			for (int j = 0; j < samplingY; j++) {
				float translationY = j * 512 / (float)samplingY;
				for (int k = 0; k < samplingZ; k++) {
					float translationZ = k * 512 / (float)samplingZ;
					translate = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0, translationZ + 50.0f));
					scale = glm::scale(glm::mat4(1.0f), glm::vec3(3.0f, 3.0f, 3.0f));
					usedModelMatrix = translate * modelMatrix * scale;
					//arrowMesh.draw(&arrowShader, camera, usedModelMatrix, materialDiffuse);
				}
			}
		}

		//glm::vec3 v = glm::vec3(1.0f, 1.0f, 1.0f);
		//// vertical angle
		//glm::vec3 vXY = glm::vec3(v.x, v.y, 0.0f);
		//float angle = glm::degrees(glm::angle(glm::normalize(vXY), glm::vec3(1.0f, 0.0f, 0.0f)));
		//angle = glm::degrees(glm::angle(glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f)));
		//angle = glm::degrees(glm::angle(glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f)));
		//angle = glm::degrees(glm::angle(glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f)));
		//angle = glm::degrees(glm::angle(glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f)));
		//angle = glm::degrees(glm::angle(glm::normalize(glm::vec3(1.0f, -1.0f, 0.0f)), glm::vec3(1.0f, 0.0f, 0.0f)));
		//
		//glm::vec3 first = glm::normalize(glm::vec3(1.0f, 0.0f, 0.0f));
		//glm::vec3 second = glm::normalize(glm::vec3(1.0f, 0.0f, 0.0f));
		//float dot = glm::dot(first, second);
		//float det = first.x * second.y - second.x * first.y;
		//angle = glm::degrees(atan2(det, dot));

		//angle = angleBetweenVectorsVertical(first, second);

		//first = glm::normalize(glm::vec3(1.0f, 1.0f, 0.0f));
		//second = glm::normalize(glm::vec3(1.0f, 0.0f, 0.0f));
		//angle = angleBetweenVectorsVertical(first, second);

		//first = glm::normalize(glm::vec3(0.0f, 1.0f, 0.0f));
		//second = glm::normalize(glm::vec3(1.0f, 0.0f, 0.0f));
		//angle = angleBetweenVectorsVertical(first, second);

		//first = glm::normalize(glm::vec3(-1.0f, 1.0f, 0.0f));
		//second = glm::normalize(glm::vec3(1.0f, 0.0f, 0.0f));
		//angle = angleBetweenVectorsVertical(first, second);

		//first = glm::normalize(glm::vec3(-1.0f, 0.0f, 0.0f));
		//second = glm::normalize(glm::vec3(1.0f, 0.0f, 0.0f));
		//angle = angleBetweenVectorsVertical(first, second);
		//
		//first = glm::normalize(glm::vec3(-1.0f, 0.0f, 0.0f));
		//second = glm::normalize(glm::vec3(1.0f, 0.0f, 0.0f));
		//dot = glm::dot(first, second);
		//det = first.x * second.y - second.x * first.y;
		//angle = glm::degrees(atan2(det, dot));

		//angle = angleBetweenVectorsVertical(first, second);

		//first = glm::normalize(glm::vec3(1.0f, -1.0f, 0.0f));
		//second = glm::normalize(glm::vec3(1.0f, 0.0f, 0.0f));
		//angle = angleBetweenVectorsVertical(first, second);

		//first = glm::normalize(glm::vec3(0.0f, -1.0f, 0.0f));
		//second = glm::normalize(glm::vec3(1.0f, 0.0f, 0.0f));
		//angle = angleBetweenVectorsVertical(first, second);

		//first = glm::normalize(glm::vec3(-1.0f, -1.0f, 0.0f));
		//second = glm::normalize(glm::vec3(1.0f, 0.0f, 0.0f));
		//angle = angleBetweenVectorsVertical(first, second);

		//// horizontal angle
		//first = glm::normalize(glm::vec3(1.0f, 0.0f, 0.0f));
		//second = glm::normalize(glm::vec3(1.0f, 0.0f, 0.0f));
		//angle = angleBetweenVectorsHorizontal(first, second);

		//first = glm::normalize(glm::vec3(1.0f, 0.0f, 1.0f));
		//second = glm::normalize(glm::vec3(1.0f, 0.0f, 0.0f));
		//angle = angleBetweenVectorsHorizontal(first, second);

		//first = glm::normalize(glm::vec3(0.0f, 0.0f, 1.0f));
		//second = glm::normalize(glm::vec3(1.0f, 0.0f, 0.0f));
		//angle = angleBetweenVectorsHorizontal(first, second);

		//first = glm::normalize(glm::vec3(-1.0f, 0.0f, 1.0f));
		//second = glm::normalize(glm::vec3(1.0f, 0.0f, 0.0f));
		//angle = angleBetweenVectorsHorizontal(first, second);

		//first = glm::normalize(glm::vec3(-1.0f, 0.0f, 0.0f));
		//second = glm::normalize(glm::vec3(1.0f, 0.0f, 0.0f));
		//angle = angleBetweenVectorsHorizontal(first, second);

		//first = glm::normalize(glm::vec3(-1.0f, 0.0f, -1.0f));
		//second = glm::normalize(glm::vec3(1.0f, 0.0f, 0.0f));
		//angle = angleBetweenVectorsHorizontal(first, second);

		//first = glm::normalize(glm::vec3(0.0f, 0.0f, -1.0f));
		//second = glm::normalize(glm::vec3(1.0f, 0.0f, 0.0f));
		//angle = angleBetweenVectorsHorizontal(first, second);

		//first = glm::normalize(glm::vec3(1.0f, 0.0f, -1.0f));
		//second = glm::normalize(glm::vec3(1.0f, 0.0f, 0.0f));
		//angle = angleBetweenVectorsHorizontal(first, second);

		//v = glm::vec3(-1.0f, 1.0f, 0.0f);
		//float verticalAngle = glm::degrees(computeVerticalAngle(v));
		//float horizontalAngle = glm::degrees(computeHorizontalAngle(v));

		//v = glm::vec3(-1.0f, 0.0f, 0.0f);
		//verticalAngle = glm::degrees(computeVerticalAngle(v));
		//horizontalAngle = glm::degrees(computeHorizontalAngle(v));

		//v = glm::vec3(-1.0f, -1.0f, 0.0f);
		//verticalAngle = glm::degrees(computeVerticalAngle(v));
		//horizontalAngle = glm::degrees(computeHorizontalAngle(v));

		//v = glm::vec3(0.0f, -1.0f, 1.0f);
		//verticalAngle = glm::degrees(computeVerticalAngle(v));
		//horizontalAngle = glm::degrees(computeHorizontalAngle(v));

		//v = glm::vec3(-1.0f, -1.0f, -1.0f);
		//verticalAngle = glm::degrees(computeVerticalAngle(v));
		//horizontalAngle = glm::degrees(computeHorizontalAngle(v));
		//glm::vec3 vN = glm::normalize(v);

		//v = glm::vec3(1.0f, 0.0f, 0.0f);		// starting position of a vector
		//glm::mat4 verticalRotation = glm::rotate(glm::mat4(1.0f), glm::radians(verticalAngle), glm::vec3(0.0f, 0.0f, 1.0f));
		//glm::mat4 horizontalRotation = glm::rotate(glm::mat4(1.0f), glm::radians(horizontalAngle), glm::vec3(0.0f, 1.0f, 0.0f));
		//glm::vec4 vRotated = horizontalRotation * verticalRotation * glm::vec4(v, 1.0f);
		//glm::vec3 vRN = glm::normalize(glm::vec3(vRotated.x, vRotated.y, vRotated.z));
		//verticalAngle = glm::degrees(computeVerticalAngle(vRN));
		//horizontalAngle = glm::degrees(computeHorizontalAngle(vRN));

		//verticalAngle = 45.0f;
		//horizontalAngle = 180.0f;
		//verticalRotation = glm::rotate(glm::mat4(1.0f), glm::radians(verticalAngle), glm::vec3(0.0f, 0.0f, 1.0f));
		//horizontalRotation = glm::rotate(glm::mat4(1.0f), glm::radians(-horizontalAngle), glm::vec3(0.0f, 1.0f, 0.0f));
		//glm::vec4 vRotated1 = horizontalRotation * glm::vec4(v, 1.0f);
		//float y = vRotated1.x * verticalRotation[1][0] + vRotated1.y * verticalRotation[1][1] + vRotated1.z * verticalRotation[1][2] + vRotated1.w * verticalRotation[1][3];
		//vRotated = glm::transpose(verticalRotation) * vRotated1;
		//vRN = glm::normalize(glm::vec3(vRotated.x, vRotated.y, vRotated.z));
		//verticalAngle = glm::degrees(computeVerticalAngle(vRN));
		//horizontalAngle = glm::degrees(computeHorizontalAngle(vRN));

		//verticalAngle = 90.0f;
		//horizontalAngle = 0.0f;
		//horizontalRotation = glm::rotate(glm::mat4(1.0f), glm::radians(-horizontalAngle), glm::vec3(0.0f, 1.0f, 0.0f));
		//rotation = glm::rotate(horizontalRotation, glm::radians(verticalAngle), glm::vec3(0.0f, 0.0f, 1.0f));
		//vRotated = rotation * glm::vec4(v, 1.0f);
		//vRN = glm::normalize(glm::vec3(vRotated.x, vRotated.y, vRotated.z));
		//verticalAngle = glm::degrees(computeVerticalAngle(vRN));
		//horizontalAngle = glm::degrees(computeHorizontalAngle(vRN));

		vis.draw(camera);
		window.swapBuffers();
	}

	return 0;
}