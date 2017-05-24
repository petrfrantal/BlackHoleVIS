#include "LineShader.h"

LineShader::LineShader(const std::string& fileName) : Shader(fileName) {
	colorLocation = glGetAttribLocation(shaderProgram, "color");
	VPLocation = glGetUniformLocation(shaderProgram, "viewProjectionMatrix");
}

LineShader::~LineShader() {

}

GLint LineShader::getColorLoc(void) {
	return colorLocation;
}

GLint LineShader::getVPLoc(void) {
	return VPLocation;
}