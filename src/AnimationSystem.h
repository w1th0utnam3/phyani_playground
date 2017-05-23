#pragma once

#include "Simulation.h"

class AnimationSystem
{
public:
	AnimationSystem(EntityComponentSystem& ecs);

	void computeTimestep(double dt);

private:
	EntityComponentSystem& m_ecs;

	static void updateRotation(Position& position, RotationMatrices& rotation);
	static void updateInertia(RigidBodyProperties& properties, RotationMatrices& rotation);


	//static void evaluatePositionChange(double t, const State& state, const RigidBody& body);

	static Eigen::Vector3d toLocalCoordinates(const Position& position, const RotationMatrices& rotation, const Eigen::Vector3d& globalCoordinates);
	static Eigen::Vector3d toGlobalCoordinates(const Position& position, const RotationMatrices& rotation, const Eigen::Vector3d& localCoordinates);
	static JointAttachement otherJointAttachement(const JointProperties& joint, EntityType current);

	double m_time;
};
