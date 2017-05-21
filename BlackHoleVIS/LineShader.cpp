#include "LineShader.h"

LineShader::LineShader(const std::string& fileName) : Shader(fileName) {
	colorLocation = glGetAttribLocation(shaderProgram, "color");
	VPLocation = glGetUniformLocation(shaderProgram, "viewProjectionMatrix");
}

LineShader::~LineShader() {

}