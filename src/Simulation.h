#pragma once

#include <memory>

class EntityComponentSystem;
class AnimationSystem;

class AnimationLoop;
class ImGuiScene;

class Simulation
{
public:
	static EntityComponentSystem& getEntityComponentSystem();
	static AnimationSystem& getAnimationSystem();

	static AnimationLoop& getAnimationLoop();
	static ImGuiScene& getImGuiScene();

private:
	Simulation();
	~Simulation() = default;

	static Simulation m_simulation;

	std::unique_ptr<EntityComponentSystem> m_ecs;
	std::unique_ptr<AnimationSystem> m_animationSystem;

	std::unique_ptr<AnimationLoop> m_animationLoop;
	std::unique_ptr<ImGuiScene> m_imGuiScene;
};
