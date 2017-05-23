#pragma once

#include "Simulation.h"

class RenderSystem
{
public:
	RenderSystem(EntityComponentSystem& ecs);

private:
	EntityComponentSystem& m_ecs;
};
