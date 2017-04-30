#pragma once

#include <atomic>
#include <future>
#include <vector>
#include <memory>

#include <GLFW/glfw3.h>
#include <AntTweakBar.h>

#include "Scene.h"

class GflwWindow
{
public:
	GflwWindow();
	~GflwWindow();

	void executeRenderLoop();

	std::future<void> stopRenderLoop();

private:
	std::atomic<bool> m_continueRenderLoop;

	bool initialize();
	void cleanup();

	void timeStep();
	void render();

	static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

	GLFWwindow* m_window;
	TwBar* m_tweakBar;

	std::vector<std::unique_ptr<Scene>> m_scenes;
};
