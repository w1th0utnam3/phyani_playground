#pragma once

#include "GlfwWindow.h"

#include <iostream>
#include <vector>

#include "TriangleShaderScene.h"
#include "DemoScene.h"
#include "GlfwWindowManager.h"

#include "noname_tools\vector_tools.h"

GlfwWindow::GlfwWindow()
	: m_continueRenderLoop(false)
	, m_lastFrametime(0)
	, m_fps(0)
{
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

	// Request a new window from GLFW
	auto futureWindow = GlfwWindowManager::requestWindow(1024, 768, "Simulation", nullptr, nullptr);
	m_window = futureWindow.get();
	glfwSetWindowUserPointer(m_window, static_cast<void*>(this));

	auto callbackRequests = noname::tools::move_construct_vector(
		GlfwWindowManager::setMouseButtonCallback(m_window, mouse_button_callback),
		GlfwWindowManager::setCursorPosCallback(m_window, cursor_position_callback),
		GlfwWindowManager::setScrollCallback(m_window, scroll_callback),
		GlfwWindowManager::setKeyCallback(m_window, key_callback),
		GlfwWindowManager::setWindowSizeCallback(m_window, window_size_callback)
	);
	for (auto& fut : callbackRequests) fut.wait();

	const auto previousContext = glfwGetCurrentContext();
	glfwMakeContextCurrent(m_window);
	gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress));
	glfwSwapInterval(1);

	// Initialize window
	if (!initialize()) {
		std::cerr << "Simulation could not be initialized." << "\n";
		return;
	}

	glfwMakeContextCurrent(previousContext);
}

GlfwWindow::~GlfwWindow()
{
	// Cleanup simulation
	cleanup();
	// Request to destroy window but don't wait
	GlfwWindowManager::destroyWindow(m_window);

	std::cout << "(sim) Simulation done." << "\n";
}

void GlfwWindow::executeRenderLoop()
{
	if (m_continueRenderLoop) return;

	const auto previousContext = glfwGetCurrentContext();
	glfwMakeContextCurrent(m_window);

	// Run render loop
	m_continueRenderLoop = true;
	while (m_continueRenderLoop && !glfwWindowShouldClose(m_window)) {
		const double t = glfwGetTime();

		render();

		const double dt = glfwGetTime() - t;
		m_lastFrametime = dt/1000.0;
		m_fps = 60.0/dt;
	}
	m_continueRenderLoop = false;

	glfwMakeContextCurrent(previousContext);
}

std::future<void> GlfwWindow::stopRenderLoop()
{
	m_continueRenderLoop = false;
}

bool GlfwWindow::initialize()
{
	if (!TwInit(TW_OPENGL, nullptr)) {
		std::cerr << "AntTweakBar initialization failed: " << TwGetLastError() << "\n";
	}
	TwWindowSize(1024, 768);

	// Create a tweak bar
	m_tweakBar = TwNewBar("TweakBar");
	TwDefine(" GLOBAL help='MiniGL TweakBar.' "); // Message added to the help bar.
	TwDefine(" TweakBar size='250 400' position='5 5' color='96 200 224' text=dark ");
	TwAddVarCB(m_tweakBar, "Rotation", 
		TW_TYPE_QUAT4D, copyFromTw<4, double>, copyToTw<4, double>, static_cast<void*>(m_camera.rotation()->coeffs().data()),
		" label='Rotation' open help='Change the rotation.' ");
	TwAddVarRO(m_tweakBar, "Frame time", TW_TYPE_DOUBLE, &m_lastFrametime, " label='Frame time' precision=5");
	TwAddVarRO(m_tweakBar, "FPS", TW_TYPE_DOUBLE, &m_fps, " label='FPS' precision=5");

	/*
	TwAddVarRO(tweakBar, "Time", TW_TYPE_FLOAT, &m_time, " label='Time' precision=5");
	TwAddVarCB(tweakBar, "Simulate", TW_TYPE_BOOL32, setSimulateCB, getSimulateCB, NULL, "label='Simulate' key=SPACE");
	// Add callback to toggle auto-rotate mode (callback functions are defined above).
	TwAddVarCB(tweakBar, "Wireframe", TW_TYPE_BOOL32, setWireframeCB, getWireframeCB, NULL,
		" label='Wireframe' key=w help='Toggle wireframe mode.' ");
	TwAddVarCB(tweakBar, "Enable export", TW_TYPE_BOOL32, setExportCB, getExportCB, nullptr, "label='Enable export'");
	TwAddVarRW(tweakBar, "Export FPS", TW_TYPE_UINT32, &m_exportFps, "label='Export FPS'");
	*/

	glClearColor(0.95f, 0.95f, 1.0f, 1.0f);

	// Create the triangle shader scene
	//m_scenes.emplace_back(new TriangleShaderScene());
	m_scenes.emplace_back(new DemoScene());
	m_scenes.back()->setCamera(&m_camera);
	m_scenes.back()->initialize(m_window);

	return true;
}

void GlfwWindow::render()
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

void GlfwWindow::mouse_button_callback(GLFWwindow* glfwWindow, int button, int action, int mods)
{
	if (TwEventMouseButtonGLFW(button, action)) return;
	auto window = static_cast<GlfwWindow*>(glfwGetWindowUserPointer(glfwWindow));
	window->m_interaction.pressedButton = (action == GLFW_PRESS) ? button : -1;
}

void GlfwWindow::cursor_position_callback(GLFWwindow* glfwWindow, double xpos, double ypos)
{
	if (TwEventMousePosGLFW(xpos, ypos)) return;
	auto window = static_cast<GlfwWindow*>(glfwGetWindowUserPointer(glfwWindow));

	const double dx = window->m_interaction.mousePosXOld - xpos;
	const double dy = ypos - window->m_interaction.mousePosYOld;

	if (window->m_interaction.pressedButton == GLFW_MOUSE_BUTTON_LEFT) {
		window->m_camera.rotate(dy / 100, -dx / 100, 0);
	}

	window->m_interaction.mousePosXOld = xpos;
	window->m_interaction.mousePosYOld = ypos;
}

void GlfwWindow::scroll_callback(GLFWwindow* glfwWindow, double xoffset, double yoffset)
{
	auto window = static_cast<GlfwWindow*>(glfwGetWindowUserPointer(glfwWindow));
	window->m_camera.zoom(0.01*yoffset);
}

void GlfwWindow::key_callback(GLFWwindow* glfwWindow, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(glfwWindow, GLFW_TRUE);

	if (TwEventKeyGLFW(key, action)) return;
}

void GlfwWindow::window_size_callback(GLFWwindow* glfwWindow, int width, int height)
{
	TwWindowSize(width, height);
}

void GlfwWindow::cleanup()
{
	TwTerminate();
}
