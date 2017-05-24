#pragma once

#include "Scene.h"

class ImGuiScene : public Scene
{
public:
	ImGuiScene();
	virtual ~ImGuiScene();

protected:
	virtual void initializeSceneContent() override;
	virtual void cleanupSceneContent() override;
	virtual void renderSceneContent() override;
};
