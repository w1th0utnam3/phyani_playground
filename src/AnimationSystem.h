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

	static void updateRotation(RotationalAnimatedBody& rigidBody);
	static void updateInertia(RotationalAnimatedBody& rigidBody);
	static void updateStaticExternalForces(TranslationalAnimatedBody& body);
	static void updateStaticExternalTorque(RotationalAnimatedBody& body);
	static void updateConnectorPositionVelocity(const EntityComponentSystem& ecs, Connector& connector);

	static void updateRenderData(const EntityComponentSystem& ecs, EntityType entity, RenderData& renderData);
	static void updateRenderData(RenderData& renderData, const TranslationalAnimatedBody& rigidBody);
	static void updateRenderData(RenderData& renderData, const RotationalAnimatedBody& particle);
	static void updateRenderData(RenderData& renderData, const Joint& joint);
};
