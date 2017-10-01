#pragma once

#include "CommonOpenGL.h"

struct CameraState
{
	//! The current rotation of the camera.
	glm::dquat rotation;
	//! The current translation of the camera.
	glm::dvec3 translation;
	//! The current scaling of the camera.
	glm::dvec3 scaling;

	//! The current projection zoom factor.
	double zoom;
};

class Camera
{
public:
	Camera(int width, int height);

	//! Resets the camera to the identity state like after calling the constructor. Does not reset the viewport dimensions.
	void resetToIdentity();
	//! Resets the camera to the identity state like after calling the constructor. Sets the viewport to the specified values.
	void resetToIdentity(int width, int height);
	//! Resets the camera to its default state.
	void resetToDefault();
	//! Stores the current camera state as the default state of this camera.
	void setAsDefault();

	//! Sets the camera translation from the origin in world coordinates to the specified values.
	void setTranslation(double x, double y, double z);
	//! Sets the scaling of the world coordinates uniformly in all directions to the specified value.
	void setScaling(double scaling);
	//! Sets the scaling of the world coordinates to the specified values.
	void setScaling(double x, double y, double z);
	//! Sets the rotation of the world coordinates to the specified values.
	void setRotation(const glm::dquat& quat);
	//! Sets the current zoom factor to the specified value, a factor of 1.0 means no zoom.
	void setZoom(double zoom);
	//! Sets the current vieport size in pixels.
	void setViewportSize(int width, int height);

	//! Rotates the camera by the specified angle around the given axis.
	void rotate(double angle, const glm::dvec3& axis);
	//! Rotates the camera by the specified quaternion, expects a four element double array of {x,y,z,w}.
	void rotate(const glm::dquat& quat);
	//! Applies the scaling factors incrementally to each of the coordinate axes.
	void scale(double x, double y, double z);
	//! Applies the scaling factor incrementally to all coordinate axes.
	void scale(double scaling);
	//! Applies the zoom factor incrementally to the current zoom level.
	void zoom(double zoom);

	//! Returns the current view matrix.
	glm::dmat4 viewMatrix() const;
	//! Returns the current projection matrix.
	glm::dmat4 projectionMatrix() const;
	//! Returns the current viewport size.
	glm::ivec2 viewportSize() const;

	//! Returns the vector containing the translation of the camera from the origin.
	glm::dvec3 translation() const;
	//! Returns the vector containing the scaling factors of the world coordinates.
	glm::dvec3 scaling() const;
	//! Returns the quaternion representing the current rotation of the camera.
	glm::dquat rotation() const;
	//! Returns the current zoom factor.
	double zoom() const;

private:
	//! Recalculates the view matrix to represent the current camera state.
	void updateViewMatrix();
	//! Recalculates the projection matrix to represent the current camera state.
	void updateProjectionMatrix();

	//! Camera state, like rotation, translation, etc.
	CameraState m_state;
	//! Default camera state that is used to reset the camera.
	CameraState m_defaultState;
	//! Current view matrix.
	glm::dmat4 m_view;
	//! Current projection matrix.
	glm::dmat4 m_projection;
	//! The current viewport dimensions.
	glm::ivec2 m_viewportSize;
};
