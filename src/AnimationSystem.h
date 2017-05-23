#pragma once

#include "EntityComponentSystem.h"

// TODO: Check usage of chrono data type for time

class AnimationSystem
{
public:
	AnimationSystem(EntityComponentSystem& ecs);

	void initialize();
	void computeTimestep(double dt);

private:
	EntityComponentSystem& m_ecs;

	double m_time;

	void prepareNextTimestep();

	static Eigen::Vector3d toGlobalCoordinates(const Particle& particle, const Eigen::Vector3d& localCoordinates);
	static Eigen::Vector3d toGlobalCoordinates(const RigidBody& body, const Eigen::Vector3d& localCoordinates);
};
