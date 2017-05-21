#include "Camera.h"

Camera::Camera(const glm::vec3 & pos, float horizontalA, float verticalA, float fov, float aspect, float zNear, float zFar) {
	position = pos;
	horizontalAngle = glm::radians(horizontalA);
	verticalAngle = glm::radians(verticalA);
	up = glm::vec3(0.0f, 1.0f, 0.0f);
	projection = glm::perspective(fov, aspect, zNear, zFar);
}

glm::mat4 Camera::getViewProjection() {
	direction = glm::vec3(cos(horizontalAngle) * cos(verticalAngle),
		sin(verticalAngle),
		-sin(horizontalAngle) * cos(verticalAngle));
	return projection * glm::lookAt(position, position + direction, up);
}

void Camera::updateCameraAngles(float horizontalAngleDelta, float verticalAngleDelta) {
	horizontalAngle -= horizontalAngleDelta;
	// We restrict the camera vertical angle to be in [90°, -90°], bellow expressed in radians. 
	// That means the direction is at most directly up or directly down. Otherwise the scene could turn around.
	if (verticalAngle - verticalAngleDelta < RIGHTANGLERADIANS && verticalAngle - verticalAngleDelta > -RIGHTANGLERADIANS) {
		verticalAngle -= verticalAngleDelta;
	}
}

void Camera::moveCameraRightLeftUpDown(float horizontalDelta, float verticalDelta) {
	glm::vec3 right = glm::cross(direction, up);						// obtain the vector to the right as a cross product of the direction and up vectors
	position -= right * horizontalDelta;
	position += up * verticalDelta;
}

void Camera::moveCameraForwardBackward(float delta) {
	position += direction * delta;
}

glm::vec3 Camera::getCameraPosition(void) {
	return position;
}

glm::vec3 Camera::getCameraDirection(void) {
	return direction;
}
