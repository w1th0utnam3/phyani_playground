#include <iostream>

#include "GlfwWindowManager.h"
#include "RenderWindow.h"
#include "Simulation.h"

#include "ShaderTestScene.h"
#include "DemoScene.h"
#include "ImGuiScene.h"

int main()
{
	{
		// Initialize Glfw
		auto glfwScope = GlfwWindowManager::create();
		std::cout << "(main) Initialized Glfw." << "\n";

		// Create a window for the simulation
		try {
			// GLFW tries to get highest context available on the system when requesting 1.0
			// RenderWindow window(1, 0);
			RenderWindow window;

			// Create scenes for the window
			ShaderTestScene scene;
			window.addScene(&scene);
			//window.addScene(&Simulation::getAnimationScene());
			window.addScene(&Simulation::getImGuiScene());

			// Start rendering of the window, blocks the thread
			window.executeRenderLoop();

			// Cleanup all scenes, free up GL resources
			window.clearScenes();
		} catch (const std::runtime_error& e) {
			std::cerr << e.what() << "\n";
			std::cerr << "(main) Exiting..." << "\n";
		}
	}

	Simulation::getEntityComponentSystem().reset();
	std::cout << "(main) Bye." << "\n";
	std::cout << std::flush;
}
