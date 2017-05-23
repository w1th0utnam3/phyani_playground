#pragma once

#include <registry.hpp>

#include "RigidBodyComponents.h"

using EntityComponentSystem = entt::DefaultRegistry<Position, Velocity, PhysicalProperties>;
