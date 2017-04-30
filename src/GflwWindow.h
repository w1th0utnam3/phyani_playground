#pragma once

#include <thread>
#include <vector>
#include <memory>

#include <GLFW/glfw3.h>
#include <AntTweakBar.h>

#include "Scene.h"

class GflwWindow
{
public:
	//! Creates a thread with a new GflwWindow
	template <typename T>
	static std::thread GflwWindow::createWindowThread(T then)
	{
		return std::thread([&]() {
				GflwWindow window;
				then();
			});
	}

	static std::thread GflwWindow::createWindowThread()
	{
		return createWindowThread([]() {});
	}

private:
	GflwWindow();
	~GflwWindow();

	bool initialize();
	void cleanup();

	void timeStep();
	void render();

	static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

	GLFWwindow* m_window;
	TwBar* m_tweakBar;

	std::vector<std::unique_ptr<Scene>> m_scenes;
};
