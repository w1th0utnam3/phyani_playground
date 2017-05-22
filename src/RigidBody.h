#pragma once

#include <type_traits>

#include <Eigen/Geometry>

class RigidBody
{
public:
	struct Properties
	{
		double mass;
		Eigen::Vector3d inertia;
	};

	struct State
	{
		Eigen::Vector3d position;
		Eigen::Quaterniond attitude;

		Eigen::Vector3d velocity;
		Eigen::Vector3d angularVelocity;
	};

	RigidBody();
	RigidBody(const Properties& properties, const State& initialState);

	Properties properties() const;
	State state() const;

	Eigen::Matrix3d globalInertia() const;
	Eigen::Matrix3d inverseGlobalInertia() const;

	static State evaluateEquationsOfMotion(double t, const State& state, const RigidBody& body);

protected:
	bool m_static;

	Properties m_properties;
	State m_state;
};

inline RigidBody::State operator+(const RigidBody::State& a, const RigidBody::State& b)
{
	RigidBody::State result{ a.position + b.position,
		Eigen::Quaterniond(
			a.attitude.w() + b.attitude.w(), 
			a.attitude.x() + b.attitude.x(), 
			a.attitude.y() + b.attitude.y(), 
			a.attitude.z() + b.attitude.z()
		),
		a.velocity + b.velocity,
		a.angularVelocity + b.angularVelocity };
	return result;
}

template <typename T>
inline RigidBody::State operator*(T factor, const RigidBody::State& state)
{
	static_assert(std::is_arithmetic_v<T>, "Multiplication for rigid body state is only allowed for scalar types.");
	RigidBody::State result{factor*state.position, 
		Eigen::Quaterniond(
			factor*state.attitude.w(),
			factor*state.attitude.x(),
			factor*state.attitude.y(),
			factor*state.attitude.z()
		),
		factor*state.velocity, 
		factor*state.angularVelocity};
	return result;
}

template <typename T>
inline RigidBody::State operator*(const RigidBody::State& state, T factor)
{
	return factor*state;
}
