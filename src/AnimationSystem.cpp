#include "AnimationSystem.h"

#include <iostream>

#include "RigidBodyComponents.h"

AnimationSystem::AnimationSystem(EntityComponentSystem& ecs)
	: m_ecs(ecs)
	, m_time(0.0)
{
}

void AnimationSystem::computeTimestep(double dt)
{
	// Update inertia tensors and rotation matrices
	for (auto rigidBody : m_ecs.view<DynamicBody>()) {
		auto& position = m_ecs.get<Position>(rigidBody);
		auto& properties = m_ecs.get<RigidBodyProperties>(rigidBody);
		//auto& rotation = m_ecs.get<RotationMatrices>(rigidBody);

		// Normalize the quaternion and update rotation matrices
		//updateRotation(position, rotation);
		// Recompute the transformed inertia tensor
		//updateInertia(properties, rotation);
	}

	// Determine external forces
	for (auto rigidBody : m_ecs.view<DynamicBody>()) {
		const auto& position = m_ecs.get<Position>(rigidBody);
		const auto& properties = m_ecs.get<RigidBodyProperties>(rigidBody);
		auto& kinetics = m_ecs.get<Kinetics>(rigidBody);

		kinetics.force.y() += 9.81*properties.mass;

		if(m_ecs.has<Joints>(rigidBody)) {
			const auto& joints = m_ecs.get<Joints>(rigidBody);
			for(auto joint : joints.joints) {
				const auto& jointProperties = m_ecs.get<JointProperties>(joint);
				const auto& jointAttachement = otherJointAttachement(jointProperties, rigidBody);
				//const auto& otherPosition = m_ecs.get<Position>(jointAttachement.entity);
				//const auto& otherRotation = m_ecs.get<RotationMatrices>(jointAttachement.entity);

				//position.translation + toGlobalCoordinates()
			}
		}
	}

	// Apply time integration
	for(auto rigidBody : m_ecs.view<DynamicBody>()) {
		const auto& properties = m_ecs.get<RigidBodyProperties>(rigidBody);
		auto& position = m_ecs.get<Position>(rigidBody);

		// Time integration		
	}

	m_time += dt;
	std::cout << m_time << "\n";
}

void AnimationSystem::updateRotation(Position& position, RotationMatrices& properties)
{
	position.rotation.normalize();
	properties.rotationMatrix = position.rotation.toRotationMatrix();
	properties.inverseRotationMatrix = position.rotation.conjugate().toRotationMatrix();
}

void AnimationSystem::updateInertia(RigidBodyProperties& properties, RotationMatrices& rotation)
{
	properties.globalInertiaMatrix = (rotation.rotationMatrix * properties.inertia.asDiagonal()) * rotation.rotationMatrix.transpose();
	properties.inverseGlobalInertiaMatrix = (rotation.rotationMatrix * properties.inertia.cwiseInverse().asDiagonal()) * rotation.rotationMatrix.transpose();
}

Eigen::Vector3d AnimationSystem::toLocalCoordinates(const Position& position, const RotationMatrices& rotation, const Eigen::Vector3d& globalCoordinates)
{
	return rotation.rotationMatrix*globalCoordinates + position.translation;
}

Eigen::Vector3d AnimationSystem::toGlobalCoordinates(const Position& position, const RotationMatrices& rotation, const Eigen::Vector3d& localCoordinates)
{
	return rotation.inverseRotationMatrix*(localCoordinates - position.translation);
}

JointAttachement AnimationSystem::otherJointAttachement(const JointProperties& joint, EntityType current)
{
	return (joint.connection.first.entity == current) ? joint.connection.second : joint.connection.first;
}
