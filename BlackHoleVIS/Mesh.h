#pragma once

#include <glew.h>
#include "ArrowShader.h"
#include "LineShader.h"
#include "ArrowModel2.h"
#include "Camera.h"
#include <glm\glm.hpp>
#include "glm/gtc/type_ptr.hpp"

/// Enum for specifying the usage of the data loaded to VBO
enum DrawUsages {
	staticDraw,		///< data will not change often
	dynamicDraw		///< we expect the data to change often
};

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

class LineMesh {
	GLuint vao;
	GLuint positionsVBO;
	GLuint colorsVBO;
	unsigned int verticesCount;
public:
	LineMesh(LineShader * shader);
	~LineMesh(void);
	void loadBuffer(float * vertices, float * colors, int vertexCount, LineShader * shader, DrawUsages usage);
	void draw(LineShader * shader, Camera & camera);
};
