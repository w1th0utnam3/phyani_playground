#include "AnimationSystem.h"

#include <cassert>
#include <iostream>

AnimationSystem::AnimationSystem(EntityComponentSystem& ecs)
	: m_ecs(ecs)
	, m_time(0.0) {}

void AnimationSystem::initialize()
{
	prepareNextTimestep();
}

void AnimationSystem::computeTimestep(double dt)
{
	// TODO: Ensure that enough space for sub steps is available

	// Apply a force to a connected body, accumulates -> not thread safe
	static const auto applyForce = [this](Connector& connector, const Eigen::Vector3d& force)
	{
		if (m_ecs.has<Particle>(connector.parentEntity)) {
			auto& particle = m_ecs.get<Particle>(connector.parentEntity);
			particle.externalForce += force;
		} else if (m_ecs.has<RigidBody>(connector.parentEntity)) {
			auto& rigidBody = m_ecs.get<RigidBody>(connector.parentEntity);
			rigidBody.externalForce += force;
			rigidBody.externalTorque += connector.localPosition.cross(rigidBody.rotationMatrix.transpose()*force);
		} else {
			assert(false);
		}
	};

	for (auto jointEntity : m_ecs.view<Joint>()) {
		auto& joint = m_ecs.get<Joint>(jointEntity);

		if (auto dampedSpring = std::get_if<Joint::DampedSpring>(&joint.jointProperties)) {
			Eigen::Vector3d distance = joint.connectors.first.globalPosition - joint.connectors.second.globalPosition;
			Eigen::Vector3d velocityDifference = joint.connectors.first.globalVelocity - joint.connectors.second.globalVelocity;
			Eigen::Vector3d firstForce = -(dampedSpring->elasticity*(distance.norm() - dampedSpring->restLength)
										   + dampedSpring->damping*(velocityDifference.dot(distance)))*distance.normalized();

			applyForce(joint.connectors.first, firstForce);
			applyForce(joint.connectors.second, -firstForce);
		}
	}

	// Loop over rigid bodies and particles: Integrate time
	for (auto rigidBodyEntity : m_ecs.view<RigidBody>()) {
		auto& rigidBody = m_ecs.get<RigidBody>(rigidBodyEntity);

		if (!rigidBody.sleeping) {
			if(rigidBody.mass > 0) {
				const auto linearAcceleration = (1 / rigidBody.mass)*rigidBody.externalForce;
				rigidBody.linearState.linearVelocity += dt*linearAcceleration;
			}

			if (rigidBody.prinicipalInertia.sum() > 0) {
				const auto angularAcceleration = rigidBody.globalInverseInertiaMatrix*(rigidBody.externalTorque - (rigidBody.angularState.angularVelocity.cross(rigidBody.globalInertiaMatrix*rigidBody.angularState.angularVelocity)));
				rigidBody.angularState.angularVelocity += dt*angularAcceleration;
			}

			const auto& quat = rigidBody.angularState.rotation;
			const auto halfAngularVelocity = 0.5*rigidBody.angularState.angularVelocity;
			const auto quatChange = Eigen::Quaterniond(0, halfAngularVelocity.x(), halfAngularVelocity.y(), halfAngularVelocity.z())*quat;
			
			rigidBody.linearState.position += dt*rigidBody.linearState.linearVelocity;
			rigidBody.angularState.rotation = Eigen::Quaterniond(
				quat.w() + dt*quatChange.w(),
				quat.x() + dt*quatChange.x(),
				quat.y() + dt*quatChange.y(),
				quat.z() + dt*quatChange.z()
			);
		}
	}

	for (auto particleEntity : m_ecs.view<Particle>()) {
		auto& particle = m_ecs.get<Particle>(particleEntity);

		if (!particle.sleeping) {
			//std::cout << "Doing nothing with particle, mass=" << particle.mass << "\n";
		}
	}

	prepareNextTimestep();

	m_time += dt;
	//std::cout << m_time << "\n";
}

void AnimationSystem::prepareNextTimestep()
{
	// Update the quaternions and rotation matrices of rigid bodies and reset forces
	for (auto rigidBodyEntity : m_ecs.view<RigidBody>()) {
		auto& rigidBody = m_ecs.get<RigidBody>(rigidBodyEntity);

		rigidBody.angularState.rotation.normalize();
		rigidBody.rotationMatrix = rigidBody.angularState.rotation.matrix();

		// Compute transformed inertia matrices
		if(rigidBody.prinicipalInertia.sum() > 0) {
			rigidBody.globalInertiaMatrix =
				(rigidBody.rotationMatrix * rigidBody.prinicipalInertia.asDiagonal()) * rigidBody.rotationMatrix.transpose();
			rigidBody.globalInverseInertiaMatrix =
				(rigidBody.rotationMatrix * rigidBody.prinicipalInertia.cwiseInverse().asDiagonal()) * rigidBody.rotationMatrix.transpose();
		}

		// Add gravity and reset torque
		rigidBody.externalForce = Eigen::Vector3d(0.0, -9.81 * rigidBody.mass, 0.0);
		rigidBody.externalTorque = Eigen::Vector3d(0.0, 0.0, 0.0);
	}

	// Reset forces for particles
	for (auto particleEntity : m_ecs.view<Particle>()) {
		auto& particle = m_ecs.get<Particle>(particleEntity);
		particle.externalForce = Eigen::Vector3d(0.0, -9.81 * particle.mass, 0.0);
	}

	// Lambda to update the global position/velocity of a connector, thread safe
	static const auto updateConnector = [this](Connector& connector)
	{
		if (m_ecs.has<Particle>(connector.parentEntity)) {
			const auto& particle = m_ecs.get<Particle>(connector.parentEntity);
			connector.globalPosition = toGlobalCoordinates(particle, connector.localPosition);
			connector.globalVelocity = particle.linearState.linearVelocity;
		} else if (m_ecs.has<RigidBody>(connector.parentEntity)) {
			const auto& rigidBody = m_ecs.get<RigidBody>(connector.parentEntity);
			connector.globalPosition = toGlobalCoordinates(rigidBody, connector.localPosition);
			connector.globalVelocity = rigidBody.rotationMatrix*(rigidBody.angularState.angularVelocity.cross(connector.localPosition))
				+ rigidBody.linearState.linearVelocity;
		} else {
			assert(false);
		}
	};

	// Loop over all joints to update connector positions/velocities
	for (auto jointEntity : m_ecs.view<Joint>()) {
		auto& joint = m_ecs.get<Joint>(jointEntity);
		updateConnector(joint.connectors.first);
		updateConnector(joint.connectors.second);
	}
}

Eigen::Vector3d AnimationSystem::toGlobalCoordinates(const Particle& particle, const Eigen::Vector3d& localCoordinates)
{
	return localCoordinates + particle.linearState.position;
}

Eigen::Vector3d AnimationSystem::toGlobalCoordinates(const RigidBody& body, const Eigen::Vector3d& localCoordinates)
{
	return body.rotationMatrix * localCoordinates + body.linearState.position;
}
