#include "VisMath.h"

void VisMath::interpolateVectors(float * first, float * second, float * result, float parameter) {
	for (int i = 0; i < 3; i++) {
		result[i] = (1 - parameter) * first[i] + parameter * second[i];
	}
}

float VisMath::computeHorizontalAngle(glm::vec3 v) {
	return 0.0f;
}
float VisMath::computeVerticalAngle(glm::vec3 v) {
	return 0.0f;
}

float VisMath::angleBetweenVectorsHorizontal(glm::vec3 first, glm::vec3 second) {
	return 0.0f;
}

float VisMath::angleBetweenVectorsVertical(glm::vec3 first, glm::vec3 second) {
	return 0.0f;
}