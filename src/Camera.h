#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

class Camera
{
public:
	Camera(int width, int height);

	void rotate(double angle, glm::dvec3 axis);
	void rotate(const void* quat);
	void setZoom(double zoom);
	void setViewportSize(int width, int height);

	glm::dmat4 modelViewMatrix() const;
	glm::dmat4 projectionMatrix() const;
	glm::ivec2 viewportSize() const;

	glm::tquat<double> rotation() const;
	void rotation(void* quat) const;
	double zoom() const;

private:
	void updateModelViewMatrix();
	void updateProjectionMatrix();

	// TODO: Annotate with world/camera coordinate system

	glm::dmat4 m_modelView;
	glm::tquat<double> m_rotation;
	glm::dvec3 m_translation;
	glm::dvec3 m_scaling;

	glm::dmat4 m_projection;
	double m_zoom;
	glm::ivec2 m_viewportSize;
};
