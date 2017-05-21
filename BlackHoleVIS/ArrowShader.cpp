#include "ArrowShader.h"

ArrowShader::ArrowShader(const std::string& fileName) : Shader(fileName) {
	MVPLocation = glGetUniformLocation(shaderProgram, "MVP");
	normalLocation = glGetAttribLocation(shaderProgram, "normal");
	modelMatrixLocation = glGetUniformLocation(shaderProgram, "modelMatrix");
	normalMatrixLocation = glGetUniformLocation(shaderProgram, "normalMatrix");
	pointLight1PositionLocation = glGetUniformLocation(shaderProgram, "pointLight1Position");
	pointLight2PositionLocation = glGetUniformLocation(shaderProgram, "pointLight2Position");
	materialDiffuseLocation = glGetUniformLocation(shaderProgram, "materialDiffuse");
}

ArrowShader::~ArrowShader() {

}

GLint ArrowShader::getNormalLoc(void) {
	return normalLocation;
}

GLint ArrowShader::getMVPLoc(void) {
	return MVPLocation;
}

GLint ArrowShader::getModelMatrixLoc(void) {
	return modelMatrixLocation;
}

GLint ArrowShader::getNormalMatrixLoc(void) {
	return normalLocation;
}

GLint ArrowShader::getLight1PositionLoc(void) {
	return pointLight1PositionLocation;
}

GLint ArrowShader::getLight2PositionLoc(void) {
	return pointLight2PositionLocation;
}

GLint ArrowShader::getMaterialDiffuseLoc(void) {
	return materialDiffuseLocation;
}