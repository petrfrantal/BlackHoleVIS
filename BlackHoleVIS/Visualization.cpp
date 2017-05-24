#include "Visualization.h"
#include <iostream>

Visualization::Visualization(ArrowShader * arrShader, LineShader * lShader) {
	arrowMesh = new ArrowMesh(arrShader);
	arrowShader = arrShader;
	lineMesh = new LineMesh(lShader);
	lineShader = lShader;
	borderMesh = new LineMesh(lShader);
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
	loadBorders();
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
	delete arrowMesh;
	delete lineMesh;
	delete borderMesh;
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
	endCuttingPlane();
	samplingParameter = samplingRate;
	interpolateData();
	std::cout << "Cutting plane disabled" << std::endl;
	std::cout << "Sampling parameter set to " << samplingParameter << std::endl;
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
	glm::vec3 secondLineMaterial;
	glm::vec3 secondMaterialCorrection = glm::vec3(0.4f);
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
	float correctionAngle = glm::radians(90.0f);
	glm::mat4 correctionRotation = glm::mat4(cos(correctionAngle), sin(correctionAngle), 0.0f, 0.0f,
		-sin(correctionAngle), cos(correctionAngle), 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f);
	glm::vec4 firstPoint = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	glm::vec4 secondPoint = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
	glm::vec4 point;
	int vertexCount = samplingParameter * samplingParameter * samplingParameter * 2;
	float * positions = new float[vertexCount * 3];
	float * colors = new float[vertexCount * 3];
	int positionIndex = 0;
	int colorIndex = 0;
	glm::vec3 white = glm::vec3(1.0f);
	glm::mat4 lineScale = glm::scale(glm::mat4(1.0f), glm::vec3(30.0f));
	for (int i = 0; i < samplingParameter; i++) {
		for (int j = 0; j < samplingParameter; j++) {
			for (int k = 0; k < samplingParameter; k++) {
				norm = norms[i][j][k] / maximalNorm;		// normalized norm - parameter for interpolation of the material
				material = (1 - norm) * firstMaterial + norm * secondMaterial;
				materials[i][j][k] = material;
				secondLineMaterial = material + secondMaterialCorrection;
				point = modelMatrices[i][j][k] * correctionRotation * lineScale * firstPoint;
				positions[positionIndex++] = point.x;
				positions[positionIndex++] = point.y;
				positions[positionIndex++] = point.z;
				colors[colorIndex++] = secondLineMaterial.x;
				colors[colorIndex++] = secondLineMaterial.y;
				colors[colorIndex++] = secondLineMaterial.z;
				point = modelMatrices[i][j][k] * correctionRotation * lineScale * secondPoint;
				positions[positionIndex++] = point.x;
				positions[positionIndex++] = point.y;
				positions[positionIndex++] = point.z;
				colors[colorIndex++] = material.x;
				colors[colorIndex++] = material.y;
				colors[colorIndex++] = material.z;
			}
		}
	}
	lineMesh->loadBuffer(positions, colors, vertexCount, lineShader, dynamicDraw);
	delete[] positions;
	delete[] colors;
}

void Visualization::loadLinesCutPlane(void) {
	float correctionAngle = glm::radians(90.0f);
	glm::mat4 correctionRotation = glm::mat4(cos(correctionAngle), sin(correctionAngle), 0.0f, 0.0f,
		-sin(correctionAngle), cos(correctionAngle), 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f);
	glm::vec4 firstPoint = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	glm::vec4 secondPoint = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
	glm::vec4 point;
	int vertexCount = samplingParameter * samplingParameter * 2;
	float * positions = new float[vertexCount * 3];
	float * colors = new float[vertexCount * 3];
	int positionIndex = 0;
	int colorIndex = 0;
	glm::vec3 white = glm::vec3(1.0f);
	glm::vec3 secondMaterial;
	glm::vec3 secondMaterialCorrection = glm::vec3(0.4f);
	glm::mat4 lineScale = glm::scale(glm::mat4(1.0f), glm::vec3(30.0f));
	glm::mat4 modelMatrix;
	glm::vec3 material;
	for (int i = 0; i < samplingParameter; i++) {
		for (int j = 0; j < samplingParameter; j++) {
			if (plane == xPlane) {
				modelMatrix = modelMatrices[i][j][planePosition];
				material = materials[i][j][planePosition];
			} else if (plane == yPlane) {
				modelMatrix = modelMatrices[i][planePosition][j];
				material = materials[i][planePosition][j];
			} else {
				modelMatrix = modelMatrices[planePosition][i][j];
				material = materials[planePosition][i][j];
			}
			secondMaterial = material + secondMaterialCorrection;
			point = modelMatrix * correctionRotation * lineScale * firstPoint;
			positions[positionIndex++] = point.x;
			positions[positionIndex++] = point.y;
			positions[positionIndex++] = point.z;
			colors[colorIndex++] = secondMaterial.x;
			colors[colorIndex++] = secondMaterial.y;
			colors[colorIndex++] = secondMaterial.z;
			point = modelMatrix * correctionRotation * lineScale * secondPoint;
			positions[positionIndex++] = point.x;
			positions[positionIndex++] = point.y;
			positions[positionIndex++] = point.z;
			colors[colorIndex++] = material.x;
			colors[colorIndex++] = material.y;
			colors[colorIndex++] = material.z;
		}
	}

	lineMesh->loadBuffer(positions, colors, vertexCount, lineShader, dynamicDraw);
	delete[] positions;
	delete[] colors;
}

void Visualization::draw(Camera & camera) {
	if (type == 0) {
		if (cuttingPlane) {
			drawArrowsCuttingPlane(camera);
		} else {
			drawArrows(camera);
		}
	} else if (type == 1) {
		drawLines(camera);
	}
	drawBorders(camera);
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
	glLineWidth(3.0f);
	lineMesh->draw(lineShader, camera);
}

void Visualization::setCuttingPlane(Planes pl, double position) {
	cuttingPlane = true;
	plane = pl;
	planePosition = position * samplingParameter;
	loadLinesCutPlane();
	if (plane == xPlane) {
		std::cout << "Cutting plane set to X plane" << std::endl;
	} else if (plane == yPlane) {
		std::cout << "Cutting plane set to Y plane" << std::endl;
	} else {
		std::cout << "Cutting plane set to Z plane" << std::endl;
	}
	std::cout << "Cutting plane position is " << planePosition << " out of " << samplingParameter << std::endl;
}

void Visualization::endCuttingPlane() {
	cuttingPlane = false;
}

void Visualization::changeModel(void) {
	if (type == 0) {
		type = 1;
	} else {
		type = 0;
	}
}

void Visualization::loadBorders() {
	int vertexCount = 72;
	float * vertices = new float[vertexCount];
	float * colors = new float[vertexCount];
	float borderDimension = imageGridDimension + 70.0f;
	float xTranslation, yTranslation, zTranslation;
	int verticesIndex = 0;
	int colorsIndex = 0;
	float offset = 49.0f;
	float additionalTranslationZ = 50.0f;
	for (int i = 0; i < 2; i++) {
		xTranslation = i * borderDimension;
		for (int j = 0; j < 2; j++) {
			yTranslation = j * borderDimension;
			for (int k = 0; k < 2; k++) {
				zTranslation = k * borderDimension;
				vertices[verticesIndex++] = xTranslation - offset;
				vertices[verticesIndex++] = yTranslation - offset;
				vertices[verticesIndex++] = zTranslation - offset + additionalTranslationZ;
				colors[colorsIndex++] = 0.7f;
				colors[colorsIndex++] = 0.7f;
				colors[colorsIndex++] = 0.7f;
			}
		}
	}
	for (int i = 0; i < 2; i++) {
		xTranslation = i * borderDimension;
		for (int j = 0; j < 2; j++) {
			zTranslation = j * borderDimension;
			for (int k = 0; k < 2; k++) {
				yTranslation = k * borderDimension;
				vertices[verticesIndex++] = xTranslation - offset;
				vertices[verticesIndex++] = yTranslation - offset;
				vertices[verticesIndex++] = zTranslation - offset + additionalTranslationZ;
				colors[colorsIndex++] = 0.7f;
				colors[colorsIndex++] = 0.7f;
				colors[colorsIndex++] = 0.7f;
			}
		}
	}
	for (int i = 0; i < 2; i++) {
		yTranslation = i * borderDimension;
		for (int j = 0; j < 2; j++) {
			zTranslation = j * borderDimension;
			for (int k = 0; k < 2; k++) {
				xTranslation = k * borderDimension;
				vertices[verticesIndex++] = xTranslation - offset;
				vertices[verticesIndex++] = yTranslation - offset;
				vertices[verticesIndex++] = zTranslation - offset + additionalTranslationZ;
				colors[colorsIndex++] = 0.7f;
				colors[colorsIndex++] = 0.7f;
				colors[colorsIndex++] = 0.7f;
			}
		}
	}
	borderMesh->loadBuffer(vertices, colors, vertexCount, lineShader, staticDraw);
	delete[] vertices;
	delete[] colors;
}

void Visualization::drawBorders(Camera & camera) {
	glLineWidth(1.0f);
	borderMesh->draw(lineShader, camera);
}