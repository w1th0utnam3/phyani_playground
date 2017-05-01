#include "Camera.h"

#include <algorithm>
#include <cmath>
#include <iostream>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/geometric.hpp>

Camera::Camera(int width, int height)
	: m_modelView()
	, m_rotation(1, 0, 0, 0)
	, m_translation(0, 0, -1)
	, m_scaling(160, 160, 160)
	, m_projection()
	, m_viewportSize(width, height)
{
	updateModelViewMatrix();
	updateProjectionMatrix();
}

void Camera::rotate(double angle, glm::dvec3 axis)
{
	glm::dvec3 rotatedAxis = glm::inverse(m_rotation)*axis;
	m_rotation = glm::rotate(m_rotation, angle, rotatedAxis);
	bool resok = (std::isfinite(m_rotation[0]) 
				&& std::isfinite(m_rotation[1]) 
				&& std::isfinite(m_rotation[2]) 
				&& std::isfinite(m_rotation[3]));
	if (!resok) throw std::runtime_error("Illegal quat");
	updateModelViewMatrix();
}

void Camera::rotate(const void* quat)
{
	std::memcpy(glm::value_ptr(m_rotation), quat, 4 * sizeof(double));
	updateModelViewMatrix();
}

void Camera::setViewportSize(int width, int height)
{
	m_viewportSize.x = width;
	m_viewportSize.y = height;
	updateProjectionMatrix();
}

glm::dmat4 Camera::modelViewMatrix() const
{
	return m_modelView;
}

glm::dmat4 Camera::projectionMatrix() const
{
	return m_projection;
}

glm::ivec2 Camera::viewportSize() const
{
	return m_viewportSize;
}

glm::tquat<double> Camera::rotation() const
{
	return m_rotation;
}

void Camera::rotation(void* quat) const
{
	std::memcpy(quat, glm::value_ptr(m_rotation), 4 * sizeof(double));
}

void Camera::updateModelViewMatrix()
{
	m_modelView = glm::translate(glm::dmat4(), m_translation);
	m_modelView = glm::scale(m_modelView, m_scaling);
	m_modelView = glm::rotate(m_modelView, glm::angle(m_rotation), glm::axis(m_rotation));
}

void Camera::updateProjectionMatrix()
{	
	const double diag = std::hypot(m_viewportSize.x, m_viewportSize.y);
	m_projection = glm::ortho<double>(-0.5*m_viewportSize.x, 0.5*m_viewportSize.x, 
									  -0.5*m_viewportSize.y, 0.5*m_viewportSize.y, -diag, diag);
}
