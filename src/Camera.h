#pragma once

#include <Eigen/Geometry>

class Camera
{
public:
	Camera();

	void zoom(double zoomIncrement);
	void translate(double x, double y, double z);
	void rotate(double angleX, double angleY, double angleZ);

	Eigen::Matrix4d toModelViewMatrix();

	Eigen::Vector3d* translation();
	Eigen::Quaterniond* rotation();
	double* zoom();

private:
	// TODO: Annotate with world/camera coordinate system

	Eigen::Vector3d m_translation;
	Eigen::Quaterniond m_rotation;
	double m_zoom;
};
