#include "Camera.h"

#include <cmath>
#include <iostream>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "MathHelper.h"

Camera::Camera(int width, int height)
{
	resetToIdentity(width, height);
}

void Camera::resetToIdentity()
{
	m_state.m_rotation = glm::dquat(1, 0, 0, 0);
	m_state.m_translation = glm::dvec3(0, 0, 0);
	m_state.m_scaling = glm::dvec3(1, 1, 1);
	m_state.m_zoom = 1.0;

	updateViewMatrix();
	updateProjectionMatrix();
}

void Camera::resetToIdentity(int width, int height)
{
	m_state.m_rotation = glm::dquat(1, 0, 0, 0);
	m_state.m_translation = glm::dvec3(0, 0, 0);
	m_state.m_scaling = glm::dvec3(1, 1, 1);
	m_state.m_zoom = 1.0;

	m_viewportSize.x = width;
	m_viewportSize.y = height;

	updateViewMatrix();
	updateProjectionMatrix();
}

void Camera::resetToDefault()
{
	m_state = m_defaultState;
	updateViewMatrix();
	updateProjectionMatrix();
}

void Camera::setAsDefault()
{
	m_defaultState = m_state;
}

void Camera::setTranslation(double x, double y, double z)
{
	m_state.m_translation = glm::dvec3(x, y, z);
	updateViewMatrix();
}

void Camera::setScaling(double x, double y, double z)
{
	m_state.m_scaling = glm::dvec3(x, y, z);
	updateViewMatrix();
}

void Camera::setZoom(double zoom)
{
	m_state.m_zoom = zoom;
	updateViewMatrix();
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

	glm::dquat rotatedAxisQuat = glm::conjugate(m_state.m_rotation) * glm::dquat(0, glm::normalize(axis));
	glm::dvec3 rotatedAxis(rotatedAxisQuat[0], rotatedAxisQuat[1], rotatedAxisQuat[2]);
	m_state.m_rotation = glm::normalize(glm::rotate(m_state.m_rotation, angle, rotatedAxis));

	updateViewMatrix();
}

void Camera::rotate(const double* quat)
{
	std::memcpy(glm::value_ptr(m_state.m_rotation), static_cast<const void*>(quat), 4 * sizeof(double));
	updateViewMatrix();
}

void Camera::zoom(double zoom)
{
	m_state.m_zoom *= zoom;
	updateViewMatrix();
	updateProjectionMatrix();
}

glm::dmat4 Camera::viewMatrix() const
{
	return m_view;
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

void Camera::updateViewMatrix()
{
	m_view = glm::translate(glm::dmat4(1.0), -m_state.m_translation);
	m_view = glm::scale(m_view, m_state.m_zoom*m_state.m_scaling);
	m_view = glm::rotate(m_view, glm::angle(m_state.m_rotation), glm::axis(m_state.m_rotation));
}

void Camera::updateProjectionMatrix()
{	
	const double ratio = m_viewportSize.x / static_cast<double>(m_viewportSize.y);
	// const double zoomFactor = 1.0 / m_state.m_zoom;

	m_projection = glm::perspective<double>(glm::radians(60.0), ratio, 0.1, 100.0);

	/*
	m_projection = glm::ortho<double>(-ratio * zoomFactor, ratio * zoomFactor,
									  -1.0 * zoomFactor, 1.0 * zoomFactor,
									  0.1, 100.0);
	*/
}
