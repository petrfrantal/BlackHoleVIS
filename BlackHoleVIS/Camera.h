//----------------------------------------------------------------------------------------
/**
* \file       Camera.h
* \author     Petr Frantal
* \date       2017/02/10
*
*  Defines the class handling the OpenGL camera function.
*
*/
//----------------------------------------------------------------------------------------

#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

#define RIGHTANGLERADIANS 1.5707f

/**
*	Representation of camera. Defines the viewing.
*/
class Camera {
private:
	/// Position of the camera.
	glm::vec3 position;

	/// Direction of the camera
	glm::vec3 direction;

	/// Camera's up vector
	glm::vec3 up;

	/// Projection matrix defined by the camera
	glm::mat4 projection;

	/// Angle of the camera in horizontal plane	(describes the rotation of the vertical y axis)
	float horizontalAngle;

	/// Angle of the camera in vertical plane (whether the camera looks up or down)
	float verticalAngle;
public:

	/**
	*	Creates the camera and sets it to the given parameters.
	*/
	Camera(const glm::vec3 & pos, float horizontalA, float verticalA, float fov, float aspect, float zNear, float zFar);

	/**
	*	Returns the projection matrix defined by the camera
	*/
	glm::mat4 getViewProjection(void);

	/**
	*	Updates the camera angles which makes the camera rotate.
	*/
	void updateCameraAngles(float horizontalAngleDelta, float verticalAngleDelta);

	/**
	*	Moves the camera to it's sides (left/right) or up and down.
	*/
	void moveCameraRightLeftUpDown(float horizontalDelta, float verticalDelta);

	/**
	*	Moves the camera forwards of backwards (i.e. adds the camera direction vector to it's position).
	*/
	void moveCameraForwardBackward(float delta);

	/**
	*	Returns the vector of a camera's position.
	*/
	glm::vec3 getCameraPosition(void);

	glm::vec3 getCameraDirection(void);
};