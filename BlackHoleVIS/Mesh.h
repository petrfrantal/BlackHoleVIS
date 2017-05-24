#pragma once

#include <glew.h>
#include "ArrowShader.h"
#include "ArrowModel2.h"
#include "Camera.h"
#include <glm\glm.hpp>
#include "glm/gtc/type_ptr.hpp"

class ArrowMesh {
private:
	GLuint vao;
	GLuint vbo;
	GLuint ebo;
	unsigned int arrowTriangleCount;
public:
	ArrowMesh(ArrowShader * shader);
	~ArrowMesh();
	void draw(ArrowShader * shader, Camera & camera, glm::mat4 modelMatrix, glm::vec3 materialDiffuse);
};
