#include "RigidBody.h"

RigidBody::RigidBody()
	: m_static(true)
{
}

RigidBody::RigidBody(const Properties& properties, const State& initialState)
	: m_static(false)
	, m_properties(properties)
	, m_state(initialState)
{
}

RigidBody::Properties RigidBody::properties() const
{
	return m_properties;
}

RigidBody::State RigidBody::state() const
{
	return m_state;
}

Eigen::Matrix3d RigidBody::globalInertia() const
{
	return (m_state.attitude * m_properties.inertia.asDiagonal()) * m_state.attitude;
}

Eigen::Matrix3d RigidBody::inverseGlobalInertia() const
{
	Eigen::Matrix3d rotation = m_state.attitude.toRotationMatrix();
	return (rotation * m_properties.inertia.cwiseInverse().asDiagonal()) * rotation;
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
	stateChange.attitude = angularVelocityQuat*state.attitude;

	// Linear acceleration
	Eigen::Vector3d force = Eigen::Vector3d(0, 0, 0);
	stateChange.velocity = (1/body.m_properties.mass)*force;
	
	// Rotational acceleration
	Eigen::Vector3d torque = Eigen::Vector3d(0, 0, 0);
	stateChange.angularVelocity = body.inverseGlobalInertia()*(torque - state.angularVelocity.cross(body.globalInertia()*state.angularVelocity));

	return stateChange;
}
