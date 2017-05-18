#include "RenderWindow.h"

#include <iostream>
#include <vector>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/geometric.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "DemoScene.h"
#include "GlfwWindowManager.h"

#include "noname_tools\vector_tools.h"

#define DEFAULT_GL_MAJOR 4
#define DEFAULT_GL_MINOR 3

RenderWindow::RenderWindow()
	: RenderWindow(DEFAULT_GL_MAJOR, DEFAULT_GL_MINOR)
{
}

RenderWindow::RenderWindow(int glVersionMajor, int glVersionMinor)
	: m_continueRenderLoop(false)
	, m_drawMode(GL_FILL)
	, m_lastFrametime(0)
	, m_fps(0)
	, m_camera(1280, 720)
{
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, glVersionMajor);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, glVersionMinor);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);

	// Request a new window from GLFW
	auto futureWindow = GlfwWindowManager::requestWindow(m_camera.viewportSize().x, m_camera.viewportSize().y, "Simulation", nullptr, nullptr);
	m_window = futureWindow.get();
	glfwSetWindowUserPointer(m_window, static_cast<void*>(this));

	auto callbackRequests = noname::tools::move_construct_vector(
		GlfwWindowManager::setMouseButtonCallback(m_window, mouse_button_callback),
		GlfwWindowManager::setCursorPosCallback(m_window, cursor_position_callback),
		GlfwWindowManager::setScrollCallback(m_window, scroll_callback),
		GlfwWindowManager::setKeyCallback(m_window, key_callback),
		GlfwWindowManager::setCharModsCallback(m_window, charmods_callback),
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

	m_camera.setTranslation(0, 0, -1);
	m_camera.setScaling(160, 160, 160);
	m_camera.setAsDefault();
}

RenderWindow::~RenderWindow()
{
	// Unregister callbacks
	auto callbackRequests = noname::tools::move_construct_vector(
		GlfwWindowManager::setMouseButtonCallback(m_window, nullptr),
		GlfwWindowManager::setCursorPosCallback(m_window, nullptr),
		GlfwWindowManager::setScrollCallback(m_window, nullptr),
		GlfwWindowManager::setKeyCallback(m_window, nullptr),
		GlfwWindowManager::setCharModsCallback(m_window, nullptr),
		GlfwWindowManager::setWindowSizeCallback(m_window, nullptr)
	);
	if (GlfwWindowManager::isInitialized()) for (auto& fut : callbackRequests) fut.wait();

	// Cleanup simulation
	cleanup();
	// Request to destroy window but don't wait
	GlfwWindowManager::destroyWindow(m_window);

	std::cout << "(sim) Simulation done." << "\n";
}

void RenderWindow::executeRenderLoop()
{
	if (m_continueRenderLoop) return;

	const auto previousContext = glfwGetCurrentContext();
	glfwMakeContextCurrent(m_window);

	// Run render loop
	m_continueRenderLoop = true;
	double tRef = glfwGetTime();
	while (m_continueRenderLoop && !glfwWindowShouldClose(m_window)) {
		const double tFrameStart = glfwGetTime();

		render();

		const double tFrameEnd = glfwGetTime();
		const double dt = tFrameEnd - tFrameStart;
		m_lastFrametime = dt*1000.0;
		m_fps = 1/dt;

		static constexpr double dtMin = 4*(1.0 / 60);
		if (tFrameEnd - tRef > dtMin) {
			tRef = tFrameEnd;
			TwRefreshBar(m_tweakBar);
		}
	}
	m_continueRenderLoop = false;

	glfwMakeContextCurrent(previousContext);
}

void RenderWindow::requestStopRenderLoop()
{
	m_continueRenderLoop = false;
}

bool RenderWindow::initialize()
{
	if (!TwInit(TW_OPENGL, nullptr)) {
		std::cerr << "AntTweakBar initialization failed: " << TwGetLastError() << "\n";
	}
	TwWindowSize(m_camera.viewportSize().x, m_camera.viewportSize().y);

	auto fromTwRotationCallback = [](const void* twData, void* userPointer)
	{
		auto window = static_cast<RenderWindow*>(userPointer);
		window->m_camera.rotate(static_cast<const double*>(twData));
	};

	auto toTwRotationCallback = [](void* twData, void* userPointer)
	{
		auto window = static_cast<RenderWindow*>(userPointer);
		window->m_camera.rotation(static_cast<double*>(twData));
	};

	auto fromTwWireframeCallback = [](const void* twData, void* userPointer)
	{
		auto window = static_cast<RenderWindow*>(userPointer);
		window->m_drawMode = (*static_cast<const bool*>(twData)) ? GL_LINE : GL_FILL;
	};

	auto toTwWireframeCallback = [](void* twData, void* userPointer)
	{
		auto window = static_cast<RenderWindow*>(userPointer);
		*static_cast<bool*>(twData) = (window->m_drawMode == GL_LINE);
	};

	// Create a tweak bar
	m_tweakBar = TwNewBar("TweakBar");
	TwDefine(" GLOBAL help='MiniGL TweakBar.' ");
	TwDefine(" TweakBar size='250 400' position='5 5' color='96 200 224' text=dark ");
	TwAddVarCB(m_tweakBar, "Rotation", 
		TW_TYPE_QUAT4D, fromTwRotationCallback, toTwRotationCallback, static_cast<void*>(this),
		" label='Rotation' open help='Change the rotation.' ");
	TwAddVarRO(m_tweakBar, "Frame time (ms)", TW_TYPE_DOUBLE, &m_lastFrametime, " label='Frame time (ms)' precision=2");
	TwAddVarRO(m_tweakBar, "FPS", TW_TYPE_DOUBLE, &m_fps, " label='FPS' precision=2");
	TwAddVarCB(m_tweakBar, "Wireframe", TW_TYPE_BOOLCPP, fromTwWireframeCallback, toTwWireframeCallback, static_cast<void*>(this),
		" label='Wireframe' key=w help='Toggle wireframe mode.' ");

	/*
	TwAddVarRO(tweakBar, "Time", TW_TYPE_FLOAT, &m_time, " label='Time' precision=5");
	TwAddVarCB(tweakBar, "Simulate", TW_TYPE_BOOL32, setSimulateCB, getSimulateCB, NULL, "label='Simulate' key=SPACE");
	// Add callback to toggle auto-rotate mode (callback functions are defined above).
	TwAddVarCB(tweakBar, "Enable export", TW_TYPE_BOOL32, setExportCB, getExportCB, nullptr, "label='Enable export'");
	TwAddVarRW(tweakBar, "Export FPS", TW_TYPE_UINT32, &m_exportFps, "label='Export FPS'");
	*/

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glClearColor(0.95f, 0.95f, 1.0f, 1.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	// Create the triangle shader scene
	//m_scenes.emplace_back(new TriangleShaderScene());
	m_scenes.emplace_back(new DemoScene());
	m_scenes.back()->setCamera(&m_camera);
	m_scenes.back()->initialize(m_window);

	return true;
}

void RenderWindow::render()
{
	glViewport(0, 0, m_camera.viewportSize().x, m_camera.viewportSize().y);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glPolygonMode(GL_FRONT_AND_BACK, m_drawMode);

	//! Render all scenes
	for (auto& scene : m_scenes) {
		if (scene != nullptr) scene->render();
	}

	TwDraw();

	glfwSwapBuffers(m_window);
}

void RenderWindow::mouse_button_callback(GLFWwindow* glfwWindow, int button, int action, int mods)
{
	if (TwEventMouseButtonGLFW3(glfwWindow, button, action, mods)) return;
	auto window = static_cast<RenderWindow*>(glfwGetWindowUserPointer(glfwWindow));

	window->m_interaction.pressedButton = (action == GLFW_PRESS) ? button : -1;

	static double xpos, ypos;
	glfwGetCursorPos(glfwWindow, &xpos, &ypos);
	window->m_interaction.lastMousePos.x = xpos;
	window->m_interaction.lastMousePos.y = ypos;
}

void RenderWindow::cursor_position_callback(GLFWwindow* glfwWindow, double xpos, double ypos)
{
	if (TwEventCursorPosGLFW3(glfwWindow, xpos, ypos)) return;
	auto window = static_cast<RenderWindow*>(glfwGetWindowUserPointer(glfwWindow));
	const int width = window->m_camera.viewportSize().x;
	const int height = window->m_camera.viewportSize().y;

	if (window->m_interaction.pressedButton == GLFW_MOUSE_BUTTON_LEFT) {
		glm::dvec3 oldMouseVec(  window->m_interaction.lastMousePos.x - 0.5*width,
								-(window->m_interaction.lastMousePos.y - 0.5*height), 0);
		glm::dvec3 newMouseVec(   xpos - 0.5*width,
								-(ypos - 0.5*height), 0);

		const double arcballRadius = std::hypot(0.5*width, 0.5*height);
		oldMouseVec /= arcballRadius;
		newMouseVec /= arcballRadius;

		if (glm::length(oldMouseVec) < 1) {
			oldMouseVec.z = std::sqrt(1.0 - std::pow(glm::length(oldMouseVec), 2));
		} else return;

		if (glm::length(newMouseVec) < 1) {
			newMouseVec.z = std::sqrt(1.0 - std::pow(glm::length(newMouseVec), 2));
		} else newMouseVec = glm::normalize(newMouseVec);

		const double angle = std::acos(std::min(1.0, glm::dot(oldMouseVec, newMouseVec)));
		const glm::dvec3 axis = glm::cross(oldMouseVec, newMouseVec);

		window->m_camera.rotate(angle, axis);
	}

	window->m_interaction.lastMousePos.x = xpos;
	window->m_interaction.lastMousePos.y = ypos;
}

void RenderWindow::scroll_callback(GLFWwindow* glfwWindow, double xoffset, double yoffset)
{
	if (TwEventScrollGLFW3(glfwWindow, xoffset, yoffset)) return;

	auto window = static_cast<RenderWindow*>(glfwGetWindowUserPointer(glfwWindow));
	window->m_camera.zoom((yoffset > 0) ? 1.1 : 0.9);
}

void RenderWindow::key_callback(GLFWwindow* glfwWindow, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(glfwWindow, GLFW_TRUE);

	if (TwEventKeyGLFW3(glfwWindow, key, scancode, action, mods)) return;
}

void RenderWindow::charmods_callback(GLFWwindow* glfwWindow, unsigned int codepoint, int mods)
{
	if (TwEventCharModsGLFW3(glfwWindow, codepoint, mods)) return;
}

void RenderWindow::window_size_callback(GLFWwindow* glfwWindow, int width, int height)
{
	auto window = static_cast<RenderWindow*>(glfwGetWindowUserPointer(glfwWindow));
	window->m_camera.setViewportSize(width, height);

	TwWindowSize(width, height);
}

void RenderWindow::cleanup()
{
	TwTerminate();
}
