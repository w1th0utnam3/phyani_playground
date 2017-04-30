#pragma once

#include "Simulation.h"

#include <iostream>
#include <thread>

#include "TriangleShaderScene.h"
#include "GlfwWindowManager.h"

namespace Simulation
{
	thread_local SimulationManager* SimulationManager::simulation = nullptr;

	// Creates a thread with a new SimulationManager
	std::thread createSimulationThread() { return std::thread(SimulationManager::start); }

	SimulationManager::SimulationManager()
	{
		// Store pointer to simulation in thread variable
		SimulationManager::simulation = this;

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

		// Initialize window context
		auto futureWindow = GlfwWindowManager::requestWindow(1024, 768, "Simulation", nullptr, nullptr);
		m_window = futureWindow.get();

		glfwSetKeyCallback(m_window, key_callback);
		glfwMakeContextCurrent(m_window);
		gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress));
		glfwSwapInterval(1);

		// Initialize simulation
		if (!initialize()) {
			std::cerr << "Simulation could not be initialized." << "\n";
			return;
		}

		// Run render loop
		while (!glfwWindowShouldClose(m_window)) {
			render();
		}
		glfwMakeContextCurrent(nullptr);
		std::cout << "(sim) Simulation done." << "\n";
	}

	SimulationManager::~SimulationManager()
	{
		// Cleanup simulation
		cleanup();
		// Request to destroy window but don't wait
		GlfwWindowManager::destroyWindow(m_window);
		GlfwWindowManager::exitEventLoop();
	}

	bool SimulationManager::initialize()
	{
		if (!TwInit(TW_OPENGL, nullptr)) {
			std::cerr << "AntTweakBar initialization failed: " << TwGetLastError() << "\n";
		}
		TwWindowSize(1024, 768);

		// Create a tweak bar
		auto tweakBar = TwNewBar("TweakBar");
		TwDefine(" GLOBAL help='MiniGL TweakBar.' "); // Message added to the help bar.
		TwDefine(" TweakBar size='250 400' position='5 5' color='96 200 224' text=dark "); // change default tweak bar size and color
		/*
		TwAddVarRO(tweakBar, "Time", TW_TYPE_FLOAT, &m_time, " label='Time' precision=5");

		TwAddVarCB(tweakBar, "Rotation", TW_TYPE_QUAT4F, setRotationCB, getRotationCB, &m_quat,
			" label='Rotation' open help='Change the rotation.' ");

		TwAddVarCB(tweakBar, "Simulate", TW_TYPE_BOOL32, setSimulateCB, getSimulateCB, NULL, "label='Simulate' key=SPACE");
		// Add callback to toggle auto-rotate mode (callback functions are defined above).
		TwAddVarCB(tweakBar, "Wireframe", TW_TYPE_BOOL32, setWireframeCB, getWireframeCB, NULL,
			" label='Wireframe' key=w help='Toggle wireframe mode.' ");
		TwAddVarCB(tweakBar, "Enable export", TW_TYPE_BOOL32, setExportCB, getExportCB, nullptr, "label='Enable export'");
		TwAddVarRW(tweakBar, "Export FPS", TW_TYPE_UINT32, &m_exportFps, "label='Export FPS'");
		*/
		m_tweakBar = tweakBar;

		glClearColor(0.95f, 0.95f, 1.0f, 1.0f);

		// Create the triangle shader scene
		m_scenes.emplace_back(new TriangleShaderScene());
		m_scenes.back()->initialize(m_window);

		return true;
	}

	void SimulationManager::render()
	{
		static int width, height;
		glfwGetFramebufferSize(m_window, &width, &height);

		glViewport(0, 0, width, height);
		glClear(GL_COLOR_BUFFER_BIT);

		//! Render all scenes
		for (auto& scene : m_scenes) {
			if (scene != nullptr) scene->render();
		}

		TwDraw();

		glfwSwapBuffers(m_window);
	}

	void SimulationManager::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
			glfwSetWindowShouldClose(window, GLFW_TRUE);
	}

	void SimulationManager::cleanup()
	{
	}

	void SimulationManager::start()
	{
		SimulationManager sim;
	}

	const SimulationManager* SimulationManager::getCurrentSimulationManager()
	{
		return simulation;
	}
}
