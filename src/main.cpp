#include <iostream>

#include "GlfwWindowManager.h"
#include "GlfwRenderWindow.h"
#include "Camera.h"

#include "Simulation.h"

#include "DemoScene.h"
#include "ImGuiScene.h"
#include "CubeShaderTestScene.h"
#include "ShaderTestScene.h"

int main()
{
	// Disable buffering for standard streams
	std::cout.setf(std::ios::unitbuf);
	std::cerr.setf(std::ios::unitbuf);

	{
		// Initialize Glfw
		auto glfwScope = GlfwWindowManager::create();
		std::cout << "(main) Initialized Glfw." << "\n";

		// Specify context settings
		ContextSettings settings;
		settings.glVersionMajor = 4;
		settings.glVersionMinor = 5;
		settings.glProfile = ContextSettings::CoreProfile;

		settings.windowWidth = 1600;
		settings.windowHeight = 900;

		// Create a window for the simulation
		try {
			// Create render window based on the context settings
			RenderWindow window(settings);
			window.setDebuggingEnabled(false);

			// Modify the camera state
			Camera* camera = window.camera();
			camera->setTranslation(0.0, 0.0, 1.0);
			camera->setScaling(0.5);
			camera->setAsDefault();

			// Create scenes for the window
			CubeShaderTestScene cube_scene;
			window.addScene(&cube_scene);
			ShaderTestScene shader_scene;
			window.addScene(&shader_scene);

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
