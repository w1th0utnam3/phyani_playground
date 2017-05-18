#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

class Camera
{
public:
	Camera(int width, int height);

	//! Rotates the camera by the specified angle around the given axis.
	void rotate(double angle, glm::dvec3 axis);
	//! Rotates the camera by the specified quaternion, expects a four element double array of {x,y,z,w}.
	void rotate(const double* quat);
	//! Sets the current zoom factor, a factor of 1.0 means no zoom.
	void setZoom(double zoom);
	//! Sets the current vieport size in pixels.
	void setViewportSize(int width, int height);

	//! Returns the current model view matrix.
	glm::dmat4 modelViewMatrix() const;
	//! Returns the current projection matrix.
	glm::dmat4 projectionMatrix() const;
	//! Returns the current viewport size.
	glm::ivec2 viewportSize() const;

	//! Returns the quaternion representing the current rotation of the camera.
	glm::tquat<double> rotation() const;
	//! Writes the quaternion values {x,y,z,w} to the specified memory location.
	void rotation(double* quat) const;
	//! Returns the current zoom factor.
	double zoom() const;

private:
	//! Recalculates the model view matrix for the current camera state.
	void updateModelViewMatrix();
	//! Recalculates the projection matrix for the current camera state.
	void updateProjectionMatrix();

	//! The current model view matrix.
	glm::dmat4 m_modelView;
	//! The current rotation of the camera.
	glm::tquat<double> m_rotation;
	//! The current translation of the camera.
	glm::dvec3 m_translation;
	//! The current scaling of the camera.
	glm::dvec3 m_scaling;

	//! The current projection matrix.
	glm::dmat4 m_projection;
	//! The current projection zoom factor.
	double m_zoom;
	//! The current viewport dimensions.
	glm::ivec2 m_viewportSize;
};
