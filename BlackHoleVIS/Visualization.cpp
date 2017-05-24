#include "Visualization.h"
#include <iostream>

Visualization::Visualization(ArrowShader * arrShader) {
	arrowMesh = new ArrowMesh(arrShader);
	arrowShader = arrShader;
	type = 0;
	cuttingPlane = false;
	// the arrays will always be of the maximum size, but we will use just samplingParameter^3 of them (reallocating would be expensive)
	modelMatrices = new glm::mat4 **[maximumSamplingParameter];
	materials = new glm::vec3 **[maximumSamplingParameter];
	norms = new float **[maximumSamplingParameter];
	for (int i = 0; i < maximumSamplingParameter; i++) {
		modelMatrices[i] = new glm::mat4 *[maximumSamplingParameter];
		materials[i] = new glm::vec3 *[maximumSamplingParameter];
		norms[i] = new float *[maximumSamplingParameter];
		for (int j = 0; j < maximumSamplingParameter; j++) {
			modelMatrices[i][j] = new glm::mat4[maximumSamplingParameter];
			materials[i][j] = new glm::vec3[maximumSamplingParameter];
			norms[i][j] = new float[maximumSamplingParameter];
		}
	}
}

Visualization::~Visualization(void) {
	for (int i = 0; i < maximumSamplingParameter; i++) {
		for (int j = 0; j < maximumSamplingParameter; j++) {
			delete[] modelMatrices[i][j];
			delete[] materials[i][j];
			delete[] norms[i][j];
		}
		delete[] modelMatrices[i];
		delete[] materials[i];
		delete[] norms[i];
	}
	delete[] modelMatrices;
	delete[] materials;
	delete[] norms;
	for (int i = 0; i < dataGridDimension; i++) {
		for (int j = 0; j < dataGridDimension; j++) {
			for (int k = 0; k < dataGridDimension; k++) {
				delete[] data[i][j][k];
			}
			delete[] data[i][j];
		}
		delete[] data[i];
	}
	delete[] data;
}

void Visualization::loadArray(const std::string & fileName) {
	std::ifstream file(fileName);

	data = new float***[dataGridDimension];

	for (int i = 0; i < dataGridDimension; i++) {
		data[i] = new float**[dataGridDimension];
		for (int j = 0; j < dataGridDimension; j++) {
			data[i][j] = new float*[dataGridDimension];
			for (int k = 0; k < dataGridDimension; k++) {
				data[i][j][k] = new float[3];
			}
		}
	}
	if (file.is_open()) {
		char comma;
		for (int i = 0; i < dataGridDimension; i++) {
			for (int j = 0; j < dataGridDimension; j++) {
				for (int k = 0; k < dataGridDimension; k++) {
					file >> data[i][j][k][0];
					//std::cout << data[i][j][k][0] << " ";
					file >> comma;
					file >> data[i][j][k][1];
					//std::cout << data[i][j][k][1] << " ";
					file >> comma;
					file >> data[i][j][k][2];
					//std::cout << data[i][j][k][2] << std::endl;
				}
			}
		}
	}
	samplingParameter = 12;		// default interpolation same as the data
	interpolateData();
}

void Visualization::setSamplingParameter(int samplingRate) {
	samplingParameter = samplingRate;
	interpolateData();
}

void Visualization::interpolateData(void) {
	// transformacni matice
	float horizontalAngle, verticalAngle;
	float defaultAngle = glm::radians(-90.0f);
	glm::mat4 defaultRotation = glm::mat4(cos(defaultAngle), sin(defaultAngle), 0.0f, 0.0f,
											-sin(defaultAngle), cos(defaultAngle), 0.0f, 0.0f,
											0.0f, 0.0f, 1.0f, 0.0f,
											0.0f, 0.0f, 0.0f, 1.0f);
	glm::mat4 vectorRotation;
	glm::mat4 rotation;
	glm::mat4 arrowScale = glm::scale(glm::vec3(0.75f));
	glm::mat4 translation;
	glm::mat4 correctionTranslation;
	glm::mat4 modelMatrix;
	glm::vec3 material;
	glm::vec3 directionNormalized;
	float translationX, translationY, translationZ;
	float translationIncrement = imageGridDimension / samplingParameter;
	glm::vec3 sampleVector;
	float norm;
	float maximalNorm = 0.0f;
	glm::vec3 firstMaterial = glm::vec3(0.5f, 0.5f, 1.0f); 
	glm::vec3 secondMaterial = glm::vec3(1.0f, 0.2f, 0.2f);

	float multiple = (float)(dataGridDimension - 1) / (samplingParameter - 1);
	int xLow, xHigh, yLow, yHigh, zLow, zHigh;
	float xMultiple = 0.0f, yMultiple = 0.0f, zMultiple = 0.0f;
	float xParameter = 0.0f, yParameter = 0.0f, zParameter = 0.0f;
	float * bfl, * bfr, * bbl, * bbr, * tfl, * tfr, * tbl, * tbr;			// the eight cells, bfl = "bottom forward left" and so on
	float bl[3], br[3], tl[3], tr[3], left[3], right[3], sample[3];

	// interpolation loop
	translationZ = 50.0f;			// default translation to get the cube slightly from the camera
	for (int i = 0; i < samplingParameter; i++) {
		zMultiple = i * multiple;
		zLow = floor(zMultiple);
		zHigh = zLow + 1;
		zParameter = zMultiple - zLow;
		translationY = 0.0f;
		for (int j = 0; j < samplingParameter; j++) {
			yMultiple = j * multiple;
			yLow = floor(yMultiple);
			yHigh = yLow + 1;
			yParameter = yMultiple - yLow;
			translationX = 0.0f;
			for (int k = 0; k < samplingParameter; k++) {
				xMultiple = k * multiple;
				xLow = floor(xMultiple);
				xHigh = xLow + 1;
				xParameter = xMultiple - xLow;
				if (xHigh == dataGridDimension) {
					xHigh = xLow;
				}
				if (yHigh == dataGridDimension) {
					yHigh = yLow;
				}
				if (zHigh == dataGridDimension) {
					zHigh = zLow;
				}
				bfl = data[zLow][yLow][xLow];
				bfr = data[zLow][yLow][xHigh];
				bbl = data[zHigh][yLow][xLow];
				bbr = data[zHigh][yLow][xHigh];
				tfl = data[zLow][yHigh][xLow];
				tfr = data[zLow][yHigh][xHigh];
				tbl = data[zHigh][yHigh][xLow];
				tbr = data[zHigh][yHigh][xHigh];
				VisMath::interpolateVectors(bfl, bbl, bl, zParameter);				// first intepolations
				VisMath::interpolateVectors(bfr, bbr, br, zParameter);
				VisMath::interpolateVectors(tfl, tbl, tl, zParameter);
				VisMath::interpolateVectors(tfr, tbr, tr, zParameter);
				VisMath::interpolateVectors(bl, tl, left, yParameter);				// second interpolations
				VisMath::interpolateVectors(br, tr, right, yParameter);
				VisMath::interpolateVectors(left, right, sample, xParameter);		// final interpolation
				// TODO: compute points for lines etc.
				sampleVector = glm::vec3(sample[0], sample[1], sample[2]);
				verticalAngle = VisMath::computeVerticalAngle(sampleVector);
				horizontalAngle = VisMath::computeHorizontalAngle(sampleVector);
				vectorRotation = VisMath::computeRotation(verticalAngle, horizontalAngle);
				rotation = vectorRotation * defaultRotation;
				translation = glm::translate(glm::mat4(1.0f), glm::vec3(translationX, translationY, translationZ));
				directionNormalized = glm::normalize(sampleVector);
				correctionTranslation = glm::translate(-100.0f * directionNormalized);
				modelMatrix = translation * rotation * arrowScale;
				modelMatrices[i][j][k] = modelMatrix;
				norm = glm::length(sampleVector);
				if (norm > maximalNorm) {
					maximalNorm = norm;
				}
				norms[i][j][k] = norm;
				translationX += translationIncrement;
			}
			translationY += translationIncrement;
		}
		translationZ += translationIncrement;
	}
	for (int i = 0; i < samplingParameter; i++) {
		for (int j = 0; j < samplingParameter; j++) {
			for (int k = 0; k < samplingParameter; k++) {
				norm = norms[i][j][k] / maximalNorm;		// normalized norm - parameter for interpolation of the material
				material = (1 - norm) * firstMaterial + norm * secondMaterial;
				materials[i][j][k] = material;
			}
		}
	}
}

void Visualization::draw(Camera & camera) {
	if (type == 0) {
		if (cuttingPlane) {
			drawArrowsCuttingPlane(camera);
		} else {
			drawArrows(camera);
		}
	} else if (type == 1) {
		if (cuttingPlane) {
			drawLinesCuttingPlane(camera);
		} else {
			drawLines(camera);
		}
	}
}

void Visualization::drawArrows(Camera & camera) {
	glm::mat4 modelMatrix;
	glm::vec3 materialDiffuse;
	for (int i = 0; i < samplingParameter; i++) {
		for (int j = 0; j < samplingParameter; j++) {
			for (int k = 0; k < samplingParameter; k++) {
				modelMatrix = modelMatrices[i][j][k];
				materialDiffuse = materials[i][j][k];
				arrowMesh->draw(arrowShader, camera, modelMatrix, materialDiffuse);
			}
		}
	}
}

void Visualization::drawArrowsCuttingPlane(Camera & camera) {
	glm::mat4 modelMatrix;
	glm::vec3 materialDiffuse;
	for (int i = 0; i < samplingParameter; i++) {
		for (int j = 0; j < samplingParameter; j++) {
			if (plane == xPlane) {
				modelMatrix = modelMatrices[i][j][planePosition];
				materialDiffuse = materials[i][j][planePosition];
			} else if (plane == yPlane) {
				modelMatrix = modelMatrices[i][planePosition][j];
				materialDiffuse = materials[i][planePosition][j];
			} else {
				modelMatrix = modelMatrices[planePosition][i][j];
				materialDiffuse = materials[planePosition][i][j];
			}
			arrowMesh->draw(arrowShader, camera, modelMatrix, materialDiffuse);
		}
	}
}

void Visualization::drawLines(Camera & camera) {

}

void Visualization::drawLinesCuttingPlane(Camera & camera) {

}