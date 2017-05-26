#pragma once

#include "Scene.h"

#include <glm/glm.hpp>

class ImGuiScene : public Scene
{
public:
	ImGuiScene();
	virtual ~ImGuiScene();

protected:
	virtual void initializeSceneContent() override;
	virtual void cleanupSceneContent() override;
	virtual void renderSceneContent() override;

private:
	struct UiOptions
	{
		bool mainUiOpen = true;

		float timestep = 0.01f;
		float timeStretch = 1.0f;
		bool automaticTimestepping = false;

		glm::fmat4 tempTransform;
	};

	UiOptions m_options;

	void drawMainWindow();
	void editTransform(glm::fmat4& matrix);
};
