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
	// TODO: Different integration schemes with sub steps
	// TODO: Ensure that enough space for sub steps is available
	// TODO: Make bodies sleep when velocity goes to zero
	// TODO: Change color of sleeping bodies

	// Apply a force to a connected body, accumulates -> not thread safe
	static const auto applyForce = [this](Connector& connector, const Eigen::Vector3d& force)
	{
		if (m_ecs.has<TranslationalAnimatedBody>(connector.parentEntity)) {
			auto& translatedBody = m_ecs.get<TranslationalAnimatedBody>(connector.parentEntity);
			translatedBody.externalForce += force;
		} 

		if (m_ecs.has<RotationalAnimatedBody>(connector.parentEntity)) {
			auto& rotatedBody = m_ecs.get<RotationalAnimatedBody>(connector.parentEntity);
			rotatedBody.externalTorque += (rotatedBody.rotationMatrix*connector.localPosition).cross(force);
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

	// Integrate equations of linear motion
	for (auto translatedEntity : m_ecs.view<TranslationalAnimatedBody>()) {
		auto& translatedBody = m_ecs.get<TranslationalAnimatedBody>(translatedEntity);

		if (!translatedBody.sleeping) {
			if (translatedBody.mass > 0) {
				const auto linearAcceleration = (1 / translatedBody.mass)*translatedBody.externalForce;
				translatedBody.state.linearVelocity += dt*linearAcceleration;
			}

			translatedBody.state.position += dt*translatedBody.state.linearVelocity;
		}
	}

	// Integrate equations of angular motion
	for (auto rotationalEntity : m_ecs.view<RotationalAnimatedBody>()) {
		auto& rotatedBody = m_ecs.get<RotationalAnimatedBody>(rotationalEntity);

		if (!rotatedBody.sleeping) {
			if (rotatedBody.prinicipalInertia.sum() > 0) {
				const auto angularAcceleration = rotatedBody.globalInverseInertiaMatrix
													* (rotatedBody.externalTorque 
														- (rotatedBody.state.angularVelocity.cross(rotatedBody.globalInertiaMatrix
																								   * rotatedBody.state.angularVelocity)
														   )
													   );
				rotatedBody.state.angularVelocity += dt*angularAcceleration;
			}

			const auto& quat = rotatedBody.state.rotation;
			const auto halfAngularVelocity = 0.5*rotatedBody.state.angularVelocity;
			const auto quatChange = Eigen::Quaterniond(0, 
													   halfAngularVelocity.x(), 
													   halfAngularVelocity.y(), 
													   halfAngularVelocity.z())*quat;

			rotatedBody.state.rotation = Eigen::Quaterniond(
				quat.w() + dt*quatChange.w(),
				quat.x() + dt*quatChange.x(),
				quat.y() + dt*quatChange.y(),
				quat.z() + dt*quatChange.z()
			);
		}
	}

	prepareNextTimestep();

	m_time += dt;
}

void AnimationSystem::prepareNextTimestep()
{
	// Reset forces for particles
	for (auto translatedEntity : m_ecs.view<TranslationalAnimatedBody>()) {
		auto& translatedBody = m_ecs.get<TranslationalAnimatedBody>(translatedEntity);
		updateStaticExternalForces(translatedBody);
	}

	// Update the quaternions and rotation matrices of rigid bodies and reset forces
	for (auto rotatedBodyEntity : m_ecs.view<RotationalAnimatedBody>()) {
		auto& rotatedBody = m_ecs.get<RotationalAnimatedBody>(rotatedBodyEntity);

		updateRotation(rotatedBody);
		updateInertia(rotatedBody);
		updateStaticExternalTorque(rotatedBody);
	}

	// Loop over all joints to update connector positions/velocities
	for (auto jointEntity : m_ecs.view<Joint>()) {
		auto& joint = m_ecs.get<Joint>(jointEntity);
		updateConnectorPositionVelocity(m_ecs, joint.connectors.first);
		updateConnectorPositionVelocity(m_ecs, joint.connectors.second);
	}

	// Update render data (i.e. positions) of entities
	for (auto renderEntity : m_ecs.view<RenderData>()) {
		auto& renderData = m_ecs.get<RenderData>(renderEntity);
		updateRenderData(m_ecs, renderEntity, renderData);
	}
}

void AnimationSystem::updateRotation(RotationalAnimatedBody& rotatedBody)
{
	// Normalize quaternion and update rotation matrix
	rotatedBody.state.rotation.normalize();
	rotatedBody.rotationMatrix = rotatedBody.state.rotation.matrix();
}

void AnimationSystem::updateInertia(RotationalAnimatedBody& rotatedBody)
{
	// Compute transformed inertia matrices
	if (rotatedBody.prinicipalInertia.sum() > 0) {
		rotatedBody.globalInertiaMatrix =
			(rotatedBody.rotationMatrix * rotatedBody.prinicipalInertia.asDiagonal()) * rotatedBody.rotationMatrix.transpose();
		rotatedBody.globalInverseInertiaMatrix =
			(rotatedBody.rotationMatrix * rotatedBody.prinicipalInertia.cwiseInverse().asDiagonal()) * rotatedBody.rotationMatrix.transpose();
	}
}

void AnimationSystem::updateStaticExternalForces(TranslationalAnimatedBody& body)
{
	body.externalForce = Eigen::Vector3d(0.0, -9.81 * body.mass, 0.0);
}

void AnimationSystem::updateStaticExternalTorque(RotationalAnimatedBody& body)
{
	body.externalTorque = Eigen::Vector3d(0.0, 0.0, 0.0);
}

void AnimationSystem::updateConnectorPositionVelocity(const EntityComponentSystem& ecs, Connector& connector)
{
	connector.globalPosition = Eigen::Vector3d(0, 0, 0);
	connector.globalVelocity = Eigen::Vector3d(0, 0, 0);

	if (ecs.has<TranslationalAnimatedBody>(connector.parentEntity)) {
		const auto& translatedBody = ecs.get<TranslationalAnimatedBody>(connector.parentEntity);
		connector.globalPosition += translatedBody.state.position;
		connector.globalVelocity += translatedBody.state.linearVelocity;
	} 

	if (ecs.has<RotationalAnimatedBody>(connector.parentEntity)) {
		const auto& rotatedBody = ecs.get<RotationalAnimatedBody>(connector.parentEntity);
		connector.globalPosition += rotatedBody.rotationMatrix*connector.localPosition;
		connector.globalVelocity += rotatedBody.state.angularVelocity.cross(rotatedBody.rotationMatrix*connector.localPosition);
	}
}

void AnimationSystem::updateRenderData(const EntityComponentSystem& ecs, EntityType entity, RenderData& renderData)
{
	if (ecs.has<TranslationalAnimatedBody>(entity)) updateRenderData(renderData, ecs.get<TranslationalAnimatedBody>(entity));
	if (ecs.has<RotationalAnimatedBody>(entity)) updateRenderData(renderData, ecs.get<RotationalAnimatedBody>(entity));
	if (ecs.has<Joint>(entity)) updateRenderData(renderData, ecs.get<Joint>(entity));
}

void AnimationSystem::updateRenderData(RenderData& renderData, const TranslationalAnimatedBody& body)
{
	if (auto cuboidData = std::get_if<RenderData::Cuboid>(&renderData.properties)) {
		cuboidData->position = body.state.position.cast<float>();
	}
}

void AnimationSystem::updateRenderData(RenderData& renderData, const RotationalAnimatedBody& body)
{
	if (auto cuboidData = std::get_if<RenderData::Cuboid>(&renderData.properties)) {
		cuboidData->rotation = body.state.rotation.cast<float>();
	}
}

void AnimationSystem::updateRenderData(RenderData& renderData, const Joint& joint)
{
	if (auto jointData = std::get_if<RenderData::Joint>(&renderData.properties)) {
		jointData->connectorPositions.first = joint.connectors.first.globalPosition.cast<float>();
		jointData->connectorPositions.second = joint.connectors.second.globalPosition.cast<float>();
	}
}
