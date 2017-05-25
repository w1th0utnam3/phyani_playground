#include "Camera.h"

#include <cmath>
#include <iostream>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "MathHelper.h"

Camera::Camera(int width, int height)
{
	resetToIdentity(width, height);
	updateModelViewMatrix();
	updateProjectionMatrix();
}

void Camera::resetToIdentity()
{
	m_state.m_rotation = glm::dquat(1, 0, 0, 0);
	m_state.m_translation = glm::dvec3(0, 0, 0);
	m_state.m_scaling = glm::dvec3(1, 1, 1);
	m_state.m_zoom = 1.0;
}

void Camera::resetToIdentity(int width, int height)
{
	resetToIdentity();
	m_viewportSize.x = width;
	m_viewportSize.y = height;
}

void Camera::resetToDefault()
{
	m_state = m_defaultState;
	updateModelViewMatrix();
	updateProjectionMatrix();
}

void Camera::setAsDefault()
{
	m_defaultState = m_state;
}

void Camera::setTranslation(double x, double y, double z)
{
	m_state.m_translation = glm::dvec3(x, y, z);
	updateModelViewMatrix();
}

void Camera::setScaling(double x, double y, double z)
{
	m_state.m_scaling = glm::dvec3(x, y, z);
	updateModelViewMatrix();
}

void Camera::setZoom(double zoom)
{
	m_state.m_zoom = zoom;
	updateProjectionMatrix();
}

void Camera::setViewportSize(int width, int height)
{
	m_viewportSize.x = width;
	m_viewportSize.y = height;
	updateProjectionMatrix();
}

void Camera::rotate(double angle, glm::dvec3 axis)
{
	if (glm::length(axis) == 0) return;

	glm::dquat rotatedAxisQuat = glm::conjugate(m_state.m_rotation)*glm::dquat(0, axis);
	glm::dvec3 rotatedAxis(rotatedAxisQuat[0], rotatedAxisQuat[1], rotatedAxisQuat[2]);
	m_state.m_rotation = glm::rotate(m_state.m_rotation, angle, glm::normalize(rotatedAxis));

	 if (!isfinite(m_state.m_rotation))
		 throw std::runtime_error("Invalid rotation");

	updateModelViewMatrix();
}

void Camera::rotate(const double* quat)
{
	std::memcpy(glm::value_ptr(m_state.m_rotation), static_cast<const void*>(quat), 4 * sizeof(double));
	updateModelViewMatrix();
}

void Camera::zoom(double zoom)
{
	m_state.m_zoom *= zoom;
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

glm::dquat Camera::rotation() const
{
	return m_state.m_rotation;
}

void Camera::rotation(double* quat) const
{
	std::memcpy(static_cast<void*>(quat), glm::value_ptr(m_state.m_rotation), 4 * sizeof(double));
}

double Camera::zoom() const
{
	return m_state.m_zoom;
}

void Camera::updateModelViewMatrix()
{
	m_modelView = glm::translate(glm::dmat4(), m_state.m_translation);
	m_modelView = glm::scale(m_modelView, m_state.m_scaling);
	m_modelView = glm::rotate(m_modelView, glm::angle(m_state.m_rotation), glm::axis(m_state.m_rotation));
}

void Camera::updateProjectionMatrix()
{	
	const double diag = std::hypot(m_viewportSize.x, m_viewportSize.y);
	m_projection = glm::ortho<double>(-0.5*m_viewportSize.x*m_state.m_zoom, 0.5*m_viewportSize.x*m_state.m_zoom,
									  -0.5*m_viewportSize.y*m_state.m_zoom, 0.5*m_viewportSize.y*m_state.m_zoom, -diag, diag);
}
