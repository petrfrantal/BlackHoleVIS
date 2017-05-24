#pragma once

#include <glm\glm.hpp>
#include <glm/gtx/vector_angle.hpp>

struct VisMath {
	static void interpolateVectors(float * first, float * second, float * result, float parameter);
	static float computeHorizontalAngle(glm::vec3 v);
	static float computeVerticalAngle(glm::vec3 v);
	static glm::mat4 computeRotation(float verticalAngleRadians, float horizontalAngleRadians);
private:
	static float angleBetweenVectorsHorizontal(glm::vec3 first, glm::vec3 second);
	static float angleBetweenVectorsVertical(glm::vec3 first, glm::vec3 second);
};
