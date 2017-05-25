#include "RenderWindow.h"

#include <iostream>
#include <vector>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/geometric.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "noname_tools\vector_tools.h"

#include "DemoScene.h"
#include "GlfwWindowManager.h"
#include "MathHelper.h"

#define DEFAULT_GL_MAJOR 4
#define DEFAULT_GL_MINOR 3

RenderWindow::RenderWindow()
	: RenderWindow(DEFAULT_GL_MAJOR, DEFAULT_GL_MINOR) {}

RenderWindow::RenderWindow(int glVersionMajor, int glVersionMinor)
	: m_continueRenderLoop(false)
	, m_drawMode(GL_FILL)
	, m_dt(0.05)
	, m_timeStretch(0.5)
	, m_camera(1920, 1080)
{
	// Specify OpenGL context profile hints
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, glVersionMajor);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, glVersionMinor);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);

	// Request a new window from GLFW
	auto futureWindow = GlfwWindowManager::requestWindow(m_camera.viewportSize().x, m_camera.viewportSize().y, "Simulation", nullptr, nullptr);
	GlfwWindowManager::processEvents();
	m_window = futureWindow.get();
	glfwSetWindowUserPointer(m_window, static_cast<void*>(this));

	// Register all callbacks
	auto callbackRequests = noname::tools::move_construct_vector(
		GlfwWindowManager::setMouseButtonCallback(m_window, mouse_button_callback),
		GlfwWindowManager::setCursorPosCallback(m_window, cursor_position_callback),
		GlfwWindowManager::setScrollCallback(m_window, scroll_callback),
		GlfwWindowManager::setKeyCallback(m_window, key_callback),
		GlfwWindowManager::setCharCallback(m_window, character_callback),
		GlfwWindowManager::setCharModsCallback(m_window, charmods_callback),
		GlfwWindowManager::setWindowSizeCallback(m_window, window_size_callback)
	);
	GlfwWindowManager::processEvents();
	for (auto& fut : callbackRequests) fut.wait();

	// Load the OpenGL functions
	const auto previousContext = glfwGetCurrentContext();
	glfwMakeContextCurrent(m_window);
	gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress));
	glfwSwapInterval(1);

	// Initialize the camera
	m_camera.setTranslation(0, 0, -1);
	m_camera.setScaling(160, 160, 160);
	m_camera.setAsDefault();

	// Initialize window
	if (!initialize()) {
		std::cerr << "Simulation could not be initialized." << "\n";
		return;
	}

	glfwMakeContextCurrent(previousContext);
}

RenderWindow::~RenderWindow()
{
	// Unregister callbacks
	auto callbackRequests = noname::tools::move_construct_vector(
		GlfwWindowManager::setMouseButtonCallback(m_window, nullptr),
		GlfwWindowManager::setCursorPosCallback(m_window, nullptr),
		GlfwWindowManager::setScrollCallback(m_window, nullptr),
		GlfwWindowManager::setKeyCallback(m_window, nullptr),
		GlfwWindowManager::setCharCallback(m_window, nullptr),
		GlfwWindowManager::setCharModsCallback(m_window, nullptr),
		GlfwWindowManager::setWindowSizeCallback(m_window, nullptr)
	);
	GlfwWindowManager::processEvents();
	if (GlfwWindowManager::isInitialized()) for (auto& fut : callbackRequests) fut.wait();

	// Cleanup simulation
	cleanup();
	// Request to destroy window
	GlfwWindowManager::destroyWindow(m_window);
	GlfwWindowManager::processEvents();

	std::cout << "(win) Window closed." << "\n";
}

bool RenderWindow::initialize()
{
	/*
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
	TwAddVarRO(m_tweakBar, "FrameTime", TW_TYPE_DOUBLE, &m_lastFrametime, " label='Frame time (ms)' precision=2");
	TwAddVarRO(m_tweakBar, "FPS", TW_TYPE_DOUBLE, &m_fps, " label='FPS' precision=2");
	TwAddVarCB(m_tweakBar, "Wireframe", TW_TYPE_BOOLCPP, fromTwWireframeCallback, toTwWireframeCallback, static_cast<void*>(this),
		" label='Wireframe' key=w help='Toggle wireframe mode.' ");
	TwAddButton(m_tweakBar, "ResetCamera", 
		[](void* userPointer) {static_cast<Camera*>(userPointer)->resetToDefault(); }, static_cast<void*>(&m_camera), 
		" label='Reset camera'");
	*/
	/*
	TwAddButton(m_tweakBar, "IncrementTime", [](void* userPointer)
	{
		auto window = static_cast<RenderWindow*>(userPointer);
		if (window->m_scenes != nullptr) {
			auto demoScene = dynamic_cast<DemoScene*>(window->m_scenes.get());
			if (demoScene != nullptr) {
				demoScene->doTimestep(window->m_dt);
			}
		}
	}, static_cast<void*>(this),
		" label='Manually increment time' key=a help='Increments the time of the physical animation.' ");
	TwAddVarRW(m_tweakBar, "IncrementStepSize", TW_TYPE_DOUBLE, &m_dt, " min=0 step=0.01 label='Manual step size' help='The step size for manual time steps.' ");
	TwAddButton(m_tweakBar, "StartStopTime", [](void* userPointer)
	{
		auto window = static_cast<RenderWindow*>(userPointer);
		if (window->m_scenes != nullptr) {
			auto demoScene = dynamic_cast<DemoScene*>(window->m_scenes.get());
			if (demoScene != nullptr) {
				demoScene->toggleAnimation(window->m_timeStretch);
			}
		}
	}, static_cast<void*>(this),
		" label='Toggle automatic timestepping' key=SPACE help='Starts or stops the physical animation.' ");
	TwAddVarRW(m_tweakBar, "TimeStretch", TW_TYPE_DOUBLE, &m_timeStretch, " min=0 step=0.01 label='Time stretch' help='Set the stretch factor from realtime to physical animation time.' ");
	TwAddButton(m_tweakBar, "ResetScene", [](void* userPointer)
	{
		auto scene = static_cast<std::unique_ptr<Scene>*>(userPointer)->get();
		if (scene != nullptr) {
			auto demoScene = dynamic_cast<DemoScene*>(scene);
			if (demoScene != nullptr) {
				demoScene->resetScene();
			}
		}
	}, static_cast<void*>(&m_scenes),
		" label='Reset scene' key=r help='Resets all physical objects.' ");
		*/

	/*
	TwAddVarRO(tweakBar, "Time", TW_TYPE_FLOAT, &m_time, " label='Time' precision=5");
	TwAddVarCB(tweakBar, "Simulate", TW_TYPE_BOOL32, setSimulateCB, getSimulateCB, NULL, "label='Simulate' key=SPACE");
	// Add callback to toggle auto-rotate mode (callback functions are defined above).
	TwAddVarCB(tweakBar, "Enable export", TW_TYPE_BOOL32, setExportCB, getExportCB, nullptr, "label='Enable export'");
	TwAddVarRW(tweakBar, "Export FPS", TW_TYPE_UINT32, &m_exportFps, "label='Export FPS'");
	*/

	return true;
}

void RenderWindow::cleanup()
{
	clearScenes();
}

void RenderWindow::addScene(Scene* scene)
{
	const auto previousContext = glfwGetCurrentContext();
	if (previousContext != m_window) glfwMakeContextCurrent(m_window);

	m_scenes.push_back(scene);
	scene->setCamera(&m_camera);
	scene->initialize(m_window);

	if (previousContext != m_window) glfwMakeContextCurrent(previousContext);
}

void RenderWindow::clearScenes()
{
	const auto previousContext = glfwGetCurrentContext();
	if (previousContext != m_window) glfwMakeContextCurrent(m_window);

	for (auto scene : m_scenes) if (scene->isInitialized()) scene->cleanup();
	m_scenes.clear();

	if (previousContext != m_window) glfwMakeContextCurrent(previousContext);
}

void RenderWindow::executeRenderLoop()
{
	if (m_continueRenderLoop) return;

	const auto previousContext = glfwGetCurrentContext();
	if (previousContext != m_window) glfwMakeContextCurrent(m_window);

	// Run render loop
	m_continueRenderLoop = true;
	while (m_continueRenderLoop && !glfwWindowShouldClose(m_window)) render();
	m_continueRenderLoop = false;

	if (previousContext != m_window) glfwMakeContextCurrent(previousContext);
}

void RenderWindow::requestStopRenderLoop()
{
	m_continueRenderLoop = false;
}

void RenderWindow::render()
{
	glViewport(0, 0, m_camera.viewportSize().x, m_camera.viewportSize().y);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPolygonMode(GL_FRONT_AND_BACK, m_drawMode);

	//! Render the current scenes
	for (auto scene : m_scenes) scene->render();

	glfwSwapBuffers(m_window);
}

void RenderWindow::mouse_button_callback(GLFWwindow* glfwWindow, int button, int action, int mods)
{
	auto window = static_cast<RenderWindow*>(glfwGetWindowUserPointer(glfwWindow));

	bool eventCaptured = false;
	for (auto scene : window->m_scenes) {
		auto sceneGlfwMouseButtonFun = scene->glfwMouseButtonFun();
		if (sceneGlfwMouseButtonFun != nullptr) eventCaptured = sceneGlfwMouseButtonFun(glfwWindow, button, action, mods);
		if (eventCaptured) break;
	}

	if (!eventCaptured) {
		window->m_interaction.pressedButton = (action == GLFW_PRESS) ? button : -1;

		static double xpos, ypos;
		glfwGetCursorPos(glfwWindow, &xpos, &ypos);
		window->m_interaction.lastMousePos.x = xpos;
		window->m_interaction.lastMousePos.y = ypos;
	}
}

void RenderWindow::cursor_position_callback(GLFWwindow* glfwWindow, double xpos, double ypos)
{
	auto window = static_cast<RenderWindow*>(glfwGetWindowUserPointer(glfwWindow));
	const int width = window->m_camera.viewportSize().x;
	const int height = window->m_camera.viewportSize().y;

	if (window->m_interaction.pressedButton == GLFW_MOUSE_BUTTON_LEFT) {
		glm::dvec3 oldMouseVec(window->m_interaction.lastMousePos.x - 0.5 * width,
		                       -(window->m_interaction.lastMousePos.y - 0.5 * height), 0);
		glm::dvec3 newMouseVec(xpos - 0.5 * width,
		                       -(ypos - 0.5 * height), 0);

		const double arcballRadius = std::hypot(0.5 * width, 0.5 * height);
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

		if (glm::length(axis) != 0) window->m_camera.rotate(angle, axis);
	}

	window->m_interaction.lastMousePos.x = xpos;
	window->m_interaction.lastMousePos.y = ypos;
}

void RenderWindow::scroll_callback(GLFWwindow* glfwWindow, double xoffset, double yoffset)
{
	auto window = static_cast<RenderWindow*>(glfwGetWindowUserPointer(glfwWindow));

	bool eventCaptured = false;
	for (auto scene : window->m_scenes) {
		auto sceneGlfwScrollFun = scene->glfwScrollFun();
		if (sceneGlfwScrollFun != nullptr) eventCaptured = sceneGlfwScrollFun(glfwWindow, xoffset, yoffset);
		if (eventCaptured) break;
	}

	if (!eventCaptured) {
		window->m_camera.zoom((yoffset > 0) ? 1.1 : 0.9);
	}
}

void RenderWindow::key_callback(GLFWwindow* glfwWindow, int key, int scancode, int action, int mods)
{
	auto window = static_cast<RenderWindow*>(glfwGetWindowUserPointer(glfwWindow));

	bool eventCaptured = false;
	for (auto scene : window->m_scenes) {
		auto sceneGlfwKeyFun = scene->glfwKeyFun();
		if (sceneGlfwKeyFun != nullptr) eventCaptured = sceneGlfwKeyFun(glfwWindow, key, scancode, action, mods);
		if (eventCaptured) break;
	}

	if (!eventCaptured) {
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) glfwSetWindowShouldClose(glfwWindow, GLFW_TRUE);
	}
}

void RenderWindow::character_callback(GLFWwindow* glfwWindow, unsigned int codepoint)
{
	auto window = static_cast<RenderWindow*>(glfwGetWindowUserPointer(glfwWindow));

	bool eventCaptured = false;
	for (auto scene : window->m_scenes) {
		auto sceneGlfwCharFun = scene->glfwCharFun();
		if (sceneGlfwCharFun != nullptr) eventCaptured = sceneGlfwCharFun(glfwWindow, codepoint);
		if (eventCaptured) break;
	}
}

void RenderWindow::charmods_callback(GLFWwindow* glfwWindow, unsigned int codepoint, int mods) {}

void RenderWindow::window_size_callback(GLFWwindow* glfwWindow, int width, int height)
{
	auto window = static_cast<RenderWindow*>(glfwGetWindowUserPointer(glfwWindow));
	window->m_camera.setViewportSize(width, height);
}
