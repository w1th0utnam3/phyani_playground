#pragma once

#include <thread>

#include "Scene.h"
#include "EntityComponentSystem.h"
#include "AnimationSystem.h"
#include "RenderSystem.h"
#include "AnimationLoop.h"

class DemoScene : public Scene
{
public:
	DemoScene(EntityComponentSystem& ecs);
	virtual ~DemoScene();

	void toggleAnimation();
	void doTimestep(double dt);

protected:
	virtual void initializeSceneContent() override;
	virtual void renderSceneContent() override;

private:
	void initializeEntities();

	static void initializeLight();
	static void drawCoordinateSystem(double axisLength);

	EntityComponentSystem& m_ecs;

	AnimationSystem m_animationSystem;
	RenderSystem m_renderSystem;

	std::thread m_animationThread;
	AnimationLoop m_animationLoop;
};
