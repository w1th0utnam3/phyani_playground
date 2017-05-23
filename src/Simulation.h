#pragma once

#include "EntityComponentSystem.h"

class Simulation
{
public:
	EntityComponentSystem& getEntityComponentSystem();

private:
	EntityComponentSystem m_ecs;
};
