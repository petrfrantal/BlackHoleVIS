#pragma once

#include "Shader.h"

class ArrowShader : public Shader {
private:
	GLint normalLocation;
	GLint MVPLocation;
	GLint modelMatrixLocation;
	GLint normalMatrixLocation;
	GLint pointLight1PositionLocation;
	GLint pointLight2PositionLocation;
	GLint materialDiffuseLocation;
public:
	ArrowShader(const std::string& fileName);
	~ArrowShader();
	GLint getNormalLoc(void);
	GLint getMVPLoc(void);
	GLint getModelMatrixLoc(void);
	GLint getNormalMatrixLoc(void);
	GLint getLight1PositionLoc(void);
	GLint getLight2PositionLoc(void);
	GLint getMaterialDiffuseLoc(void);
};
