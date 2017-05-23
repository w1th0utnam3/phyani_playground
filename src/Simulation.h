#pragma once

#include <registry.hpp>

#include "RigidBodyComponents.h"

using EntityComponentSystem = entt::StandardRegistry<EntityType,
                                                     DynamicBody,
                                                     Position,
                                                     Velocity,
                                                     Kinetics,
                                                     RigidBodyProperties,
                                                     Joints,
                                                     JointProperties>;

class Simulation
{
public:
	EntityComponentSystem& getEntityComponentSystem();

private:
	EntityComponentSystem m_ecs;
};
