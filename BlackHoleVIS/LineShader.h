#pragma once

#include "Shader.h"

class LineShader : public Shader {
private:
	GLint colorLocation;
	GLint VPLocation;
public:
	LineShader(const std::string& fileName);
	~LineShader();
	GLint getColorLoc(void);
	GLint getVPLoc(void);
};
