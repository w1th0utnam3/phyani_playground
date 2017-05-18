#include "Camera.h"

#include <cmath>
#include <iostream>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>

Camera::Camera(int width, int height)
	: m_modelView()
	, m_rotation(1, 0, 0, 0)
	, m_translation(0, 0, -1)
	, m_scaling(160, 160, 160)
	, m_projection()
	, m_zoom(1.0)
	, m_viewportSize(width, height)
{
	updateModelViewMatrix();
	updateProjectionMatrix();
}

void Camera::rotate(double angle, glm::dvec3 axis)
{
	glm::dvec3 rotatedAxis = glm::inverse(m_rotation)*axis;
	m_rotation = glm::rotate(m_rotation, angle, rotatedAxis);
	updateModelViewMatrix();
}

void Camera::rotate(const double* quat)
{
	std::memcpy(glm::value_ptr(m_rotation), static_cast<const void*>(quat), 4 * sizeof(double));
	updateModelViewMatrix();
}

void Camera::setZoom(double zoom)
{
	m_zoom = zoom;
	updateProjectionMatrix();
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

void Camera::rotation(double* quat) const
{
	std::memcpy(static_cast<void*>(quat), glm::value_ptr(m_rotation), 4 * sizeof(double));
}

double Camera::zoom() const
{
	return m_zoom;
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
	m_projection = glm::ortho<double>(-0.5*m_viewportSize.x*m_zoom, 0.5*m_viewportSize.x*m_zoom,
									  -0.5*m_viewportSize.y*m_zoom, 0.5*m_viewportSize.y*m_zoom, -diag, diag);
}
