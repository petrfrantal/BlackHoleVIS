#include <SDL.h>
#include "Slider.h"
#undef main
#include <glm\glm.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <glm\gtc\quaternion.hpp>
#include <glm\gtx\quaternion.hpp>
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

glm::mat4 computeRotation(float verticalAngleDeg, float horizontalAngleDeg) {
	float verticalAngle = glm::radians(verticalAngleDeg);
	float horizontalAngle = glm::radians(horizontalAngleDeg);
	glm::mat3 rot1 = glm::mat3(cos(verticalAngle), sin(verticalAngle), 0.0f,
		-sin(verticalAngle), cos(verticalAngle), 0.0f,
		0.0f, 0.0f, 1.0f);
	glm::mat3 rot2 = glm::mat3(cos(-horizontalAngle), 0.0f, -sin(-horizontalAngle),
		0.0f, 1.0f, 0.0f,
		sin(-horizontalAngle), 0.0f, cos(-horizontalAngle));
	glm::mat3 rot = rot2 * rot1;
	return glm::mat4(rot[0][0], rot[0][1], rot[0][2], 0.0f,
		rot[1][0], rot[1][1], rot[1][2], 0.0f,
		rot[2][0], rot[2][1], rot[2][2], 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f);
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

	Visualization vis(&arrowShader, &lineShader);
	vis.loadArray("data/el2_32_32_32b.csv");

	// Mesh definitions
	ArrowMesh arrowMesh(&arrowShader);
	LineMesh lineMesh(&lineShader);

	// create a frame slider
	Slider slider0;
	Slider slider1;

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
	int slider0ClickValue = -1;
	int slider1ClickValue = -1;
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
					num0 += 3;
					vis.setSamplingParameter(num0);
					std::cout << "Send " << num0 << std::endl;
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
					slider0ClickValue = slider0.clickSlider(e.motion.x, e.motion.y);
					slider1ClickValue = slider1.clickSlider(e.motion.x, e.motion.y);
					if (slider0ClickValue >= 0) {
						num0 = slider0ClickValue;
						leftMouseButtonSlider0 = true;
						std::cout << "NUM0 " << num0 + 3 << std::endl;
					} else if (slider1ClickValue >= 0) {
						num1 = slider1ClickValue;
						leftMouseButtonSlider1 = true;
						std::cout << "NUM1 " << num1 << std::endl;
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
					slider0ClickValue = slider0.dragSlider(e.motion.x);
					num0 = slider0ClickValue;
					slider0ClickValue = -1;
					std::cout << "NUM0 " << num0 + 3 << std::endl;
				} else if (leftMouseButtonSlider1) {
					slider1ClickValue = slider1.dragSlider(e.motion.x);
					num1 = slider1ClickValue;
					slider1ClickValue = -1;
					std::cout << "NUM1 " << num1 << std::endl;
				}
			}
		}
		window.clear(0.0f, 0.0f, 0.0f, 1.0f);
		// here will be the draws (we have to compute the transform matrices and so on - from the vector data)
		glm::mat4 translate;
		glm::mat4 scale;
		glm::mat4 modelMatrix;
		glm::vec3 materialDiffuse = glm::vec3(1.0f, 1.0f, 1.0f);
		float vectorLength = 1.0f;
		glm::vec3 redMat = glm::vec3(1.0f, 0.2f, 0.2f); 
		glm::vec3 blueMat = glm::vec3(0.7f, 0.7f, 0.7f);
		materialDiffuse = vectorLength * redMat + (1 - vectorLength) * blueMat;

		float defaultAngle = glm::radians(-90.0f);
		glm::mat4 defaultRotation = glm::mat4(cos(defaultAngle), sin(defaultAngle), 0.0f, 0.0f,
								-sin(defaultAngle), cos(defaultAngle), 0.0f, 0.0f,
								0.0f, 0.0f, 1.0f, 0.0f,
								0.0f, 0.0f, 0.0f, 1.0f);
		glm::vec3 v25 = glm::vec3(1.0f, 1.0f, 1.0f);
		float verticalAngle = glm::degrees(computeVerticalAngle(v25));
		float horizontalAngle = glm::degrees(computeHorizontalAngle(v25));
		glm::mat4 fullRotation = computeRotation(verticalAngle, horizontalAngle);
		modelMatrix = fullRotation * defaultRotation;

		int dimension = 512;
		int samplingX = 2;
		int samplingY = 1;
		int samplingZ = 1;
		glm::mat4 usedModelMatrix;

		glm::vec3 firstPoint = glm::vec3(0.0f, 0.0f, 0.0f);
		glm::vec3 secondPoint = glm::vec3(1.0f, 0.0f, 0.0f);
		glm::vec4 point;
		glm::vec3 verts[2];
		glm::vec3 colors[2];

		for (int i = 0; i < samplingX; i++) {
			float translationX = i * 100 / (float)samplingX;
			for (int j = 0; j < samplingY; j++) {
				float translationY = j * 512 / (float)samplingY;
				for (int k = 0; k < samplingZ; k++) {
					float translationZ = k * 512 / (float)samplingZ;
					translate = glm::translate(glm::mat4(1.0f), glm::vec3(translationX, 0.0f, translationZ + 50.0f));
					scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.75f));
					usedModelMatrix = translate * modelMatrix * scale;
					//arrowMesh.draw(&arrowShader, camera, usedModelMatrix, materialDiffuse);

					modelMatrix = computeRotation(verticalAngle, horizontalAngle);
					usedModelMatrix =  translate * modelMatrix * glm::scale(glm::mat4(1.0f), glm::vec3(30.0f));
					point = usedModelMatrix * glm::vec4(firstPoint, 1.0f);
					verts[0] = glm::vec3(point.x, point.y, point.z);
					colors[0] = redMat;
					point = usedModelMatrix * glm::vec4(secondPoint, 1.0f);
					verts[1] = glm::vec3(point.x, point.y, point.z);
					colors[1] = blueMat;
					//lineMesh.loadBuffer(&verts[0], 2, &colors[0], &lineShader, staticDraw);
					//lineMesh.draw(&lineShader, camera);
				}
			}
		}
		vis.draw(camera);
		slider0.drawSlider(windowWidth, windowHeight, num0, 48, 0);
		slider1.drawSlider(windowWidth, windowHeight, num1, 51, 1);
		window.swapBuffers();
	}

	return 0;
}