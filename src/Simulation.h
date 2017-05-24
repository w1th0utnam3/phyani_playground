#pragma once

#include <memory>

class EntityComponentSystem;
class AnimationSystem;
class RenderSystem;

class Simulation
{
public:
	static EntityComponentSystem& getEntityComponentSystem();
	static AnimationSystem& getAnimationSystem();
	static RenderSystem& getRenderSystem();

private:
	Simulation();
	~Simulation() = default;

	static Simulation m_simulation;

	std::unique_ptr<EntityComponentSystem> m_ecs;
	std::unique_ptr<AnimationSystem> m_animationSystem;
	std::unique_ptr<RenderSystem> m_renderSystem;
};
