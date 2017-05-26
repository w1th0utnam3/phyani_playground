#pragma once

#include <memory>

class EntityComponentSystem;
class AnimationSystem;
class RenderSystem;

class AnimationLoop;
class DemoScene;
class ImGuiScene;

class Simulation
{
public:
	static EntityComponentSystem& getEntityComponentSystem();
	static AnimationSystem& getAnimationSystem();
	static RenderSystem& getRenderSystem();

	static AnimationLoop& getAnimationLoop();
	static DemoScene& getAnimationScene();
	static ImGuiScene& getImGuiScene();

private:
	Simulation();
	~Simulation() = default;

	static Simulation m_simulation;

	std::unique_ptr<EntityComponentSystem> m_ecs;
	std::unique_ptr<AnimationSystem> m_animationSystem;
	std::unique_ptr<RenderSystem> m_renderSystem;

	std::unique_ptr<AnimationLoop> m_animationLoop;
	std::unique_ptr<DemoScene> m_demoScene;
	std::unique_ptr<ImGuiScene> m_imGuiScene;
};
