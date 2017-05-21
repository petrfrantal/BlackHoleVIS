#pragma once

#include <glew.h>
#include <glm/glm.hpp>
#include <string>
#include <iostream>
#include <fstream>
#include <string>

class Shader {
private:
	/**
	*	Loads a shader from a file to a string.
	*/
	std::string loadShader(const std::string& fileName);

	/**
	*	Checks for the errors occured while linking or validating the shader program.
	*/
	void checkShaderError(GLuint shader, GLuint flag, bool isProgram, const std::string& errorMessage);

	/**
	*	Creates a vertex or fragment shader.
	*/
	GLuint createShader(const std::string& text, unsigned int type);

protected:
	/// The shader program consisting of vertex and fragment shader.
	GLuint shaderProgram;

	///	Vertex shader
	GLuint vertexShader;

	/// Fragment shader
	GLuint fragmentShader;

	/// Location of the position input attribute.
	GLint positionLocation;

public:
	Shader(const std::string& fileName);
	~Shader(void);
	GLint getPositionLoc(void);
	GLuint getShaderProgram(void);
};
