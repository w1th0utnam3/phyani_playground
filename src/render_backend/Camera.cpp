#include "Camera.h"

#include <cmath>

Camera::Camera(int width, int height)
{
	resetToIdentity(width, height);
}

void Camera::resetToIdentity()
{
	m_state.rotation = glm::dquat(1, 0, 0, 0);
	m_state.translation = glm::dvec3(0, 0, 0);
	m_state.scaling = glm::dvec3(1, 1, 1);
	m_state.zoom = 1.0;

	updateViewMatrix();
	updateProjectionMatrix();
}

void Camera::resetToIdentity(int width, int height)
{
	m_state.rotation = glm::dquat(1, 0, 0, 0);
	m_state.translation = glm::dvec3(0, 0, 0);
	m_state.scaling = glm::dvec3(1, 1, 1);
	m_state.zoom = 1.0;

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
	m_state.translation = glm::dvec3(x, y, z);
	updateViewMatrix();
}

void Camera::setScaling(double scaling)
{
	m_state.scaling = glm::dvec3(scaling, scaling, scaling);
	updateViewMatrix();
}

void Camera::setScaling(double x, double y, double z)
{
	m_state.scaling = glm::dvec3(x, y, z);
	updateViewMatrix();
}

void Camera::setRotation(const glm::dquat& quat)
{
	m_state.rotation = quat;
	updateViewMatrix();
}

void Camera::setZoom(double zoom)
{
	m_state.zoom = zoom;
	updateViewMatrix();
	updateProjectionMatrix();
}

void Camera::setViewportSize(int width, int height)
{
	m_viewportSize.x = width;
	m_viewportSize.y = height;
	updateProjectionMatrix();
}

void Camera::rotate(double angle, const glm::dvec3& axis)
{
	if (glm::length(axis) == 0) return;

	glm::dquat rotatedAxisQuat = glm::conjugate(m_state.rotation) * glm::dquat(0, glm::normalize(axis)) * m_state.rotation;
	glm::dvec3 rotatedAxis(rotatedAxisQuat[0], rotatedAxisQuat[1], rotatedAxisQuat[2]);
	m_state.rotation = glm::normalize(glm::rotate(m_state.rotation, angle, rotatedAxis));

	updateViewMatrix();
}

void Camera::rotate(const glm::dquat& quat)
{
	m_state.rotation = quat*m_state.rotation;

	updateViewMatrix();
}

void Camera::scale(double x, double y, double z)
{
	m_state.scaling.x *= x;
	m_state.scaling.y *= y;
	m_state.scaling.z *= z;

	updateViewMatrix();
}

void Camera::scale(double scaling)
{
	m_state.scaling *= scaling;

	updateViewMatrix();
}

void Camera::zoom(double zoom)
{
	m_state.zoom *= zoom;

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

glm::dvec3 Camera::translation() const
{
	return m_state.translation;
}

glm::dvec3 Camera::scaling() const
{
	return m_state.scaling;
}

glm::dquat Camera::rotation() const
{
	return m_state.rotation;
}

double Camera::zoom() const
{
	return m_state.zoom;
}

void Camera::updateViewMatrix()
{
	m_view = glm::translate(glm::dmat4(1.0), -m_state.translation);
	m_view = glm::scale(m_view, m_state.scaling);
	m_view = glm::rotate(m_view, glm::angle(m_state.rotation), glm::axis(m_state.rotation));
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
