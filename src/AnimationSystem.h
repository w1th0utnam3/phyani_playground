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

	static void updateRotation(RigidBody& rigidBody);
	static void updateInertia(RigidBody& rigidBody);
	static void updateStaticExternalForces(RigidBody& rigidBody);
	static void updateStaticExternalForces(Particle& particle);
	static void updateConnectorPositionVelocity(const EntityComponentSystem& ecs, Connector& connector);

	static void updateRenderData(const EntityComponentSystem& ecs, EntityType entity, RenderData& renderData);
	static void updateRenderData(RenderData& renderData, const RigidBody& rigidBody);
	static void updateRenderData(RenderData& renderData, const Particle& particle);
	static void updateRenderData(RenderData& renderData, const Joint& joint);

	static Eigen::Vector3d toGlobalCoordinates(const Particle& particle, const Eigen::Vector3d& localCoordinates);
	static Eigen::Vector3d toGlobalCoordinates(const RigidBody& body, const Eigen::Vector3d& localCoordinates);
};
