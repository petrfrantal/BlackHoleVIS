#pragma once

#include <glm\glm.hpp>

struct VisMath {
	static void interpolateVectors(float * first, float * second, float * result, float parameter);
	static float computeHorizontalAngle(glm::vec3 v);
	static float computeVerticalAngle(glm::vec3 v);
private:
	static float angleBetweenVectorsHorizontal(glm::vec3 first, glm::vec3 second);
	static float angleBetweenVectorsVertical(glm::vec3 first, glm::vec3 second);
};
