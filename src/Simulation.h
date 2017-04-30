#pragma once

#include <thread>
#include <vector>
#include <memory>

#include <GLFW/glfw3.h>
#include <AntTweakBar.h>

#include "Scene.h"

namespace Simulation
{
	class SimulationManager
	{
	public:
		static void start();
		static const SimulationManager* getCurrentSimulationManager();

	private:
		thread_local static SimulationManager* simulation;

		SimulationManager();
		~SimulationManager();

		bool initialize();
		void cleanup();

		void timeStep();
		void render();

		static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

		GLFWwindow* m_window;
		TwBar* m_tweakBar;

		std::vector<std::unique_ptr<Scene>> m_scenes;
	};

	std::thread createSimulationThread();
}
