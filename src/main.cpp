#include <iostream>

#include "Simulation.h"
#include "RenderWindow.h"
#include "DemoScene.h"
#include "GlfwWindowManager.h"

int main()
{
	{
		// Initialize Glfw
		auto glfwScope = GlfwWindowManager::create();
		std::cout << "(main) Initialized Glfw." << "\n";

		Simulation simulation;
		auto& ecs = simulation.getEntityComponentSystem();

		// Create a window for the simulation
		auto windowThread = std::thread([&ecs]()
		{
			{
				RenderWindow window;
				// Create a scene for the window
				window.setScene(std::make_unique<DemoScene>(ecs));
				// Start rendering of the window
				window.executeRenderLoop();
			}
			// After the window is closed, the event loop will be stopped
			GlfwWindowManager::stopEventLoop();
		});

		// Start the event loop, this statement blocks the main thread
		GlfwWindowManager::executeEventLoop();

		// Wait until the window thread finished executing
		std::cout << "(main) Waiting for window thread..." << "\n";
		windowThread.join();
	}

	std::cout << "(main) Bye." << "\n";
	std::cout << std::flush;
}
