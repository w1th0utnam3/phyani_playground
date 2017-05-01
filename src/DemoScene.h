#pragma once

#include "Scene.h"

class DemoScene : public Scene
{
public:
	DemoScene() = default;
	~DemoScene() = default;

protected:
	virtual void initializeSceneContent();
	virtual void renderSceneContent();

private:
	static void initializeLight();
	static void drawCoordinateSystem(double axisLength);
};
