#include "Mesh.h"

ArrowMesh::ArrowMesh(ArrowShader * shader) {
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// vertices - VBO
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(arrowVertices), arrowVertices, GL_STATIC_DRAW);

	// indices - EBO
	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(arrowTriangles), arrowTriangles, GL_STATIC_DRAW);

	glEnableVertexAttribArray(shader->getPositionLoc());
	glVertexAttribPointer(shader->getPositionLoc(), 3, GL_FLOAT, GL_FALSE, arrowNAttribsPerVertex * sizeof(float), (void*)0);
	glEnableVertexAttribArray(shader->getNormalLoc());
	glVertexAttribPointer(shader->getNormalLoc(), 3, GL_FLOAT, GL_FALSE, arrowNAttribsPerVertex * sizeof(float), (void*)(3 * sizeof(float)));

	glBindVertexArray(0);
	arrowTriangleCount = arrowNTriangles;
}

ArrowMesh::~ArrowMesh() {
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ebo);
}

void ArrowMesh::draw(ArrowShader * shader, Camera & camera, glm::mat4 modelMatrix, glm::vec3 materialDiffuse) {
	glm::mat4 viewProjectionMatrix = camera.getViewProjection();
	glm::mat4 normalMatrix = glm::transpose(glm::inverse(modelMatrix));
	glm::mat4 MVP = viewProjectionMatrix * modelMatrix;
	glm::vec3 cameraPosition = camera.getCameraPosition();
	glm::vec3 cameraDirection = camera.getCameraDirection();
	glm::vec3 pointLight1Position = cameraPosition - 15.0f * cameraDirection;			// put the light behind the camera
	glm::vec3 pointLight2Position = pointLight1Position + glm::vec3(0.0f, 20.0f, 0.0f);	// and the second light above the first light
	glUseProgram(shader->getShaderProgram());
	glBindVertexArray(vao);
	glUniformMatrix4fv(shader->getMVPLoc(), 1, GL_FALSE, &MVP[0][0]);
	glUniformMatrix4fv(shader->getModelMatrixLoc(), 1, GL_FALSE, &modelMatrix[0][0]);
	glUniformMatrix4fv(shader->getNormalMatrixLoc(), 1, GL_FALSE, &normalMatrix[0][0]);
	glUniform3fv(shader->getLight1PositionLoc(), 1, glm::value_ptr(pointLight1Position));
	glUniform3fv(shader->getLight2PositionLoc(), 1, glm::value_ptr(pointLight2Position));
	glUniform3fv(shader->getMaterialDiffuseLoc(), 1, glm::value_ptr(materialDiffuse));
	glDrawElements(GL_TRIANGLES, arrowTriangleCount * 3, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	glUseProgram(0);
}