#include <iostream>

#include "GlfwWindowManager.h"
#include "GlfwRenderWindow.h"
#include "RenderExceptions.h"
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

	try {
		// Initialize GLFW
		// Cleanup of OpenGL context and GLFW is performed when the scope of the manager is left.
		auto glfwScope = GlfwWindowManager::create(true);
		std::cout << "(main) Initialized Glfw." << "\n";

		// Specify OpenGL context settings
		ContextSettings settings;
		settings.glVersionMajor = 4;
		settings.glVersionMinor = 3;
		settings.glProfile = ContextSettings::CoreProfile;

		settings.windowWidth = 1600;
		settings.windowHeight = 900;
	
		// Create render window based on the context settings
		GlfwRenderWindow window(settings);
		window.setDebuggingEnabled(true);
		window.setWireframeEnabled(false);

		// Modify the camera state to allow a better view of the scenes
		Camera* camera = window.camera();
		camera->setTranslation(0.0, 0.0, 1.0);
		camera->setScaling(0.5);
		camera->setAsDefault();

		// Add a scene which is currently under development for testing
		CubeShaderTestScene cube_scene;
		window.addScene(&cube_scene);

		// Add the scene which is responsible for rendering the animated entities
		//window.addScene(&Simulation::getAnimationScene());
		// Add the scene for the gui components to control the simulation
		window.addScene(&Simulation::getImGuiScene());

		// Start rendering of the window, blocks the thread.
		// Next statement is reached when the GLFW window was closed by the user.
		window.executeRenderLoop();

		// Cleanup all scenes, free up GL resources
		window.clearScenes();
	} catch (const GlfwError& e) {
		std::cerr << e.what() << "\n";
		std::cerr << "(main) Exiting..." << "\n";
	}

	// Cleanup all allocated entities
	Simulation::getEntityComponentSystem().reset();

	std::cout << "(main) Bye." << "\n";
	std::cout << std::flush;
}
