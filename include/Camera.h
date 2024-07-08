#ifndef CAMERA_H
#define CAMERA_H

#include <Eigen/Dense>
#include <Eigen/Geometry>
#include <iostream>

class Camera
{
private:
	// define xScale, yScale, zFar, zNear
	float xScale, yScale, zFar, zNear;
	// define camera position
	Eigen::Vector3f position;
	// define camera rotation
	Eigen::Vector3f rotation;
	// define camera up vector
	Eigen::Vector3f up;

	Eigen::Matrix4f projectionMatrix;
	Eigen::Matrix4f viewMatrix;


public:
	init();
	Camera(){};
	Camera(float xScale, float yScale, float zFar, float zNear, Eigen::Vector3f position, Eigen::Vector3f rotation, Eigen::Vector3f up):
		xScale(xScale), yScale(yScale), zFar(zFar), zNear(zNear), position(position), rotation(rotation), up(up){};


};

#endif // CAMERA_H