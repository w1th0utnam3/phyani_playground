#include "RigidBody.h"

RigidBody::RigidBody()
	: m_static(true)
{
}

RigidBody::RigidBody(const Properties& properties)
	: m_static(false)
	, m_properties(properties)
{
}

RigidBody::RigidBody(const Properties& properties, const State& initialState)
	: m_static(false)
	, m_properties(properties)
	, m_state(initialState)
{
	updateMatrices();
}

RigidBody::Properties RigidBody::properties() const
{
	return m_properties;
}

RigidBody::State RigidBody::state() const
{
	return m_state;
}

void RigidBody::setState(const RigidBody::State& state)
{
	m_state = state;
	updateMatrices();
}

Eigen::Vector3d RigidBody::toLocalCoordinates(const Eigen::Vector3d& globalCoordinates) const
{
	return m_rotationMatrix*globalCoordinates + m_state.position;
}

Eigen::Vector3d RigidBody::toGlobalCoordinates(const Eigen::Vector3d& localCoordinates) const
{
	return m_inverseRotationMatrix*(localCoordinates - m_state.position);
}

Eigen::Matrix3d RigidBody::globalInertia() const
{
	return (m_rotationMatrix * m_properties.inertia.asDiagonal()) * m_rotationMatrix.transpose();
}

Eigen::Matrix3d RigidBody::inverseGlobalInertia() const
{
	return (m_rotationMatrix * m_properties.inertia.cwiseInverse().asDiagonal()) * m_rotationMatrix.transpose();
}

RigidBody::State RigidBody::evaluateEquationsOfMotion(double t, const State& state, const RigidBody& body)
{
	State stateChange;

	// Velocity
	stateChange.position = state.velocity;

	// Attitude change
	Eigen::Quaterniond angularVelocityQuat = Eigen::Quaterniond(
		0.0, 
		0.5*state.angularVelocity(0), 
		0.5*state.angularVelocity(1), 
		0.5*state.angularVelocity(2)
	);
	stateChange.rotation = angularVelocityQuat*state.rotation;

	// Linear acceleration
	Eigen::Vector3d force = Eigen::Vector3d(0, 0, 0);
	stateChange.velocity = (1/body.m_properties.mass)*force;
	
	// Rotational acceleration
	Eigen::Vector3d torque = Eigen::Vector3d(0, 0, 0);
	stateChange.angularVelocity = body.inverseGlobalInertia()*(torque - state.angularVelocity.cross(body.globalInertia()*state.angularVelocity));

	return stateChange;
}

void RigidBody::updateMatrices()
{
	m_state.rotation.normalize();
	m_rotationMatrix = m_state.rotation.toRotationMatrix();
	m_inverseRotationMatrix = m_state.rotation.conjugate().toRotationMatrix();
}
