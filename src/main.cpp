#include <iostream>

#include "GflwWindow.h"
#include "GlfwWindowManager.h"

int main()
{
	{
		// Initialize Glfw
		auto glfwScope = GlfwWindowManager::create();
		std::cout << "(main) Initialized Glfw." << "\n";

		// Create a window for the simulation
		auto simWindow = GflwWindow::createWindowThread([]()
		{
			// After the window is closed, the event loop will be stopped
			GlfwWindowManager::exitEventLoop();
		});

		// Start the event loop, this statement blocks the main thread
		GlfwWindowManager::startEventLoop();

		// Wait until the window thread is done
		std::cout << "(main) Waiting for window thread..." << "\n";
		simWindow.join();
	}

	std::cout << "(main) Bye." << "\n";
	std::cout << std::flush;
}
