#include <iostream>

#include "RenderWindow.h"
#include "DemoScene.h"
#include "GlfwWindowManager.h"

int main()
{
	{
		// Initialize Glfw
		auto glfwScope = GlfwWindowManager::create();
		std::cout << "(main) Initialized Glfw." << "\n";

		// Create a window for the simulation
		auto windowThread = std::thread([]()
		{
			{
				RenderWindow window;
				// Create a scene for the window
				window.setScene(std::make_unique<DemoScene>());
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
