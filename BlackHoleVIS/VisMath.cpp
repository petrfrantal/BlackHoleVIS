#include "VisMath.h"

void VisMath::interpolateVectors(float * first, float * second, float * result, float parameter) {
	for (int i = 0; i < 3; i++) {
		result[i] = (1 - parameter) * first[i] + parameter * second[i];
	}
}

float VisMath::computeHorizontalAngle(glm::vec3 v) {
	if (v.x == 0.0f && v.z == 0.0f) {
		return 0.0f;
	}
	glm::vec3 vXZ = glm::vec3(v.x, 0.0f, v.z);
	vXZ = glm::normalize(vXZ);
	glm::vec3 xAxis = glm::vec3(1.0f, 0.0f, 0.0f);
	return angleBetweenVectorsHorizontal(vXZ, xAxis);
}
float VisMath::computeVerticalAngle(glm::vec3 v) {
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

float VisMath::angleBetweenVectorsHorizontal(glm::vec3 first, glm::vec3 second) {
	float dot = glm::dot(first, second);
	float det = first.x * second.z - second.x * first.z;
	float angle = glm::degrees(atan2(det, dot));
	if (angle != 0.0f) {
		angle = -angle;
	}
	return glm::radians(angle);
}

float VisMath::angleBetweenVectorsVertical(glm::vec3 first, glm::vec3 second) {
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

glm::mat4 VisMath::computeRotation(float verticalAngleRadians, float horizontalAngleRadians) {
	glm::mat3 rot1 = glm::mat3(cos(verticalAngleRadians), sin(verticalAngleRadians), 0.0f,
		-sin(verticalAngleRadians), cos(verticalAngleRadians), 0.0f,
		0.0f, 0.0f, 1.0f);
	glm::mat3 rot2 = glm::mat3(cos(-horizontalAngleRadians), 0.0f, -sin(-horizontalAngleRadians),
		0.0f, 1.0f, 0.0f,
		sin(-horizontalAngleRadians), 0.0f, cos(-horizontalAngleRadians));
	glm::mat3 rot = rot2 * rot1;
	return glm::mat4(rot[0][0], rot[0][1], rot[0][2], 0.0f,
		rot[1][0], rot[1][1], rot[1][2], 0.0f,
		rot[2][0], rot[2][1], rot[2][2], 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f);
}