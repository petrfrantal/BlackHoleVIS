#pragma once

#include <math.h>
#include "Mesh.h"
#include "ArrowShader.h"
#include "VisMath.h"

class Visualization {
private:
	float **** data;
	const int dataGridDimension = 32;
	ArrowMesh * arrowMesh;
	ArrowShader * arrowShader;
	int type;						// Arrows, lines
	bool cuttingPlane;				// Cutting plane used?
	int samplingParameter;			// how many samples in 
	const int maximumSamplingParameter = 50;
	const float imageGridDimension = 500.0f;
	glm::mat4 *** modelMatrices;		// rotation matrix of every arrow
	glm::vec3 *** materials;			// materials of every arrow
	// private methods
	void interpolateData(void);
	void drawArrows(Camera & camera);
	void drawLines(Camera & camera);
	void drawArrowsCuttingPlane(Camera & camera);
	void drawLinesCuttingPlane(Camera & camera);
public:
	Visualization(ArrowShader * arrowShader);
	~Visualization(void);
	void loadArray(const std::string & fileName);
	void setSamplingParameter(int samplingRate);
	void draw(Camera & camera);
};
