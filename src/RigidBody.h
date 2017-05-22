#pragma once

#include <type_traits>

#include <Eigen/Geometry>

class RigidBody
{
public:
	//! The physical properties of a rigid body.
	struct Properties
	{
		double mass;
		Eigen::Vector3d inertia;
	};

	//! Represents the state of a rigid body, i.e. its orientation, velocity, etc.
	struct State
	{
		Eigen::Vector3d position;
		Eigen::Quaterniond rotation;

		Eigen::Vector3d velocity;
		Eigen::Vector3d angularVelocity;
	};

	RigidBody();
	RigidBody(const Properties& properties);
	RigidBody(const Properties& properties, const State& initialState);

	Properties properties() const;
	State state() const;
	void setState(const State& state);

	Eigen::Vector3d toLocalCoordinates(const Eigen::Vector3d& globalCoordinates) const;
	Eigen::Vector3d toGlobalCoordinates(const Eigen::Vector3d& localCoordinates) const;

	Eigen::Matrix3d globalInertia() const;
	Eigen::Matrix3d inverseGlobalInertia() const;

	static State evaluateEquationsOfMotion(double t, const State& state, const RigidBody& body);

protected:
	void updateMatrices();

	bool m_static;

	Properties m_properties;
	State m_state;

	Eigen::Matrix3d m_rotationMatrix;
	Eigen::Matrix3d m_inverseRotationMatrix;
};

//! Addition of two RigidBody::State objects by adding the states componentwise. The result is not physically sensible and the operator is only useful for integrators.
inline RigidBody::State operator+(const RigidBody::State& a, const RigidBody::State& b)
{
	RigidBody::State result{ a.position + b.position,
		Eigen::Quaterniond(
			a.rotation.w() + b.rotation.w(), 
			a.rotation.x() + b.rotation.x(), 
			a.rotation.y() + b.rotation.y(), 
			a.rotation.z() + b.rotation.z()
		),
		a.velocity + b.velocity,
		a.angularVelocity + b.angularVelocity };
	return result;
}

//! Scaling of a RigidBody::State by a factor by scaling the states componentwise. The result is not physically sensible and the operator is only useful for integrators.
template <typename T>
inline RigidBody::State operator*(T factor, const RigidBody::State& state)
{
	static_assert(std::is_arithmetic_v<T>, "Multiplication for rigid body state is only allowed for scalar types.");
	RigidBody::State result{factor*state.position, 
		Eigen::Quaterniond(
			factor*state.rotation.w(),
			factor*state.rotation.x(),
			factor*state.rotation.y(),
			factor*state.rotation.z()
		),
		factor*state.velocity, 
		factor*state.angularVelocity};
	return result;
}
