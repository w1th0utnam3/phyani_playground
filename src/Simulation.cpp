#include "Simulation.h"

#include "EntityComponentSystem.h"
#include "AnimationSystem.h"
#include "AnimationLoop.h"
#include "ImGuiScene.h"

Simulation Simulation::m_simulation;

Simulation::Simulation()
	: m_ecs(std::make_unique<EntityComponentSystem>())
	, m_animationSystem(std::make_unique<AnimationSystem>(*m_ecs))
	, m_animationLoop(std::make_unique<AnimationLoop>(*m_animationSystem))
	, m_imGuiScene(std::make_unique<ImGuiScene>())
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

AnimationLoop& Simulation::getAnimationLoop()
{
	return *m_simulation.m_animationLoop;
}

ImGuiScene& Simulation::getImGuiScene()
{
	return *m_simulation.m_imGuiScene;
}
