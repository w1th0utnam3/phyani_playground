#include <iostream>

#include "RenderWindow.h"
#include "DemoScene.h"
#include "ImGuiScene.h"
#include "GlfwWindowManager.h"

int main()
{
	{
		// Initialize Glfw
		auto glfwScope = GlfwWindowManager::create();
		std::cout << "(main) Initialized Glfw." << "\n";

		// Create a window for the simulation
		{
			RenderWindow window;

			// Create scenes for the window
			auto demoScene = std::make_unique<DemoScene>();
			window.addScene(demoScene.get());

			auto imguiScene = std::make_unique<ImGuiScene>();
			window.addScene(imguiScene.get());

			// Start rendering of the window and block the thread
			window.executeRenderLoop();

			// Cleanup all scenes, free up GL resources
			window.clearScenes();
		}
		// After the window is closed, the event loop will be stopped
		GlfwWindowManager::stopEventLoop();

		// Start the event loop, this statement blocks the main thread
		//GlfwWindowManager::executeEventLoop();
	}

	std::cout << "(main) Bye." << "\n";
	std::cout << std::flush;
}
