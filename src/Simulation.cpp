#include "Simulation.h"

#include "EntityComponentSystem.h"
#include "AnimationSystem.h"
#include "RenderSystem.h"
#include "AnimationLoop.h"
#include "DemoScene.h"
#include "ImGuiScene.h"

Simulation Simulation::m_simulation;

Simulation::Simulation()
	: m_ecs(std::make_unique<EntityComponentSystem>())
	, m_animationSystem(std::make_unique<AnimationSystem>(*m_ecs))
	, m_renderSystem(std::make_unique<RenderSystem>(*m_ecs))
	, m_animationLoop(std::make_unique<AnimationLoop>(*m_animationSystem))
	, m_demoScene(std::make_unique<DemoScene>())
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

RenderSystem& Simulation::getRenderSystem()
{
	return *m_simulation.m_renderSystem;
}

AnimationLoop& Simulation::getAnimationLoop()
{
	return *m_simulation.m_animationLoop;
}

DemoScene& Simulation::getAnimationScene()
{
	return *m_simulation.m_demoScene;
}

ImGuiScene& Simulation::getImGuiScene()
{
	return *m_simulation.m_imGuiScene;
}
