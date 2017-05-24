#include "Simulation.h"

#include "EntityComponentSystem.h"
#include "AnimationSystem.h"
#include "RenderSystem.h"

Simulation Simulation::m_simulation;

Simulation::Simulation()
	: m_ecs(std::make_unique<EntityComponentSystem>())
	, m_animationSystem(std::make_unique<AnimationSystem>(*m_ecs))
	, m_renderSystem(std::make_unique<RenderSystem>(*m_ecs))
{
}

EntityComponentSystem& Simulation::getEntityComponentSystem()
{
	return *m_simulation.m_ecs;
}

AnimationSystem& Simulation::getAnimationSystem()
{
	return *m_simulation.m_animationSystem;
}

RenderSystem& Simulation::getRenderSystem()
{
	return *m_simulation.m_renderSystem;
}
