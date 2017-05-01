#include "Camera.h"

Camera::Camera()
	: m_translation(0, 0, 0)
	  , m_rotation(1, 0, 0, 0)
	  , m_zoom(1) {}

void Camera::zoom(double zoomIncrement)
{
	m_zoom += zoomIncrement;
}

void Camera::translate(double x, double y, double z)
{
	m_translation[0] += x;
	m_translation[1] += y;
	m_translation[2] += z;
}

void Camera::rotate(double angleX, double angleY, double angleZ)
{ 
	{
		Eigen::AngleAxisd angleAxisX(angleX, Eigen::Vector3d(1, 0, 0));
		Eigen::Quaterniond quatX(angleAxisX);
		m_rotation = m_rotation * quatX;
	} 
	{
		Eigen::AngleAxisd angleAxisY(angleY, Eigen::Vector3d(0, 1, 0));
		Eigen::Quaterniond quatY(angleAxisY);
		m_rotation = m_rotation * quatY;
	} 
	{
		Eigen::AngleAxisd angleAxisZ(angleY, Eigen::Vector3d(0, 0, 1));
		Eigen::Quaterniond quatZ(angleAxisZ);
		m_rotation = m_rotation * quatZ;
	}
}

Eigen::Matrix4d Camera::toModelViewMatrix()
{
	//auto rotationMatrix = m_rotation.toRotationMatrix();
	Eigen::Matrix4d transformationMatrix = Eigen::Matrix4d::Identity();
	transformationMatrix.block<3, 3>(0, 0) = m_rotation.toRotationMatrix();
	transformationMatrix.block<3, 1>(0, 3) = m_translation;
	transformationMatrix(0, 0) *= m_zoom;
	transformationMatrix(1, 1) *= m_zoom;
	transformationMatrix(2, 2) *= m_zoom;
	return transformationMatrix;
}

Eigen::Vector3d* Camera::translation()
{
	return &m_translation;
}

Eigen::Quaterniond* Camera::rotation()
{
	return &m_rotation;
}
double* Camera::zoom()
{
	return &m_zoom;
}
