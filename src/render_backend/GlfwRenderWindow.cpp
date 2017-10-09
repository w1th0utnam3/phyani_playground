#include "GlfwRenderWindow.h"

#include <iostream>
#include <vector>
#include <stdexcept>

#include <noname_tools/vector_tools.h>

#include "DemoScene.h"
#include "GlfwWindowManager.h"
#include "GlfwHelper.h"
#include "MathHelper.h"

GlfwRenderWindow::GlfwRenderWindow(const ContextSettings& settings)
	: m_continueRenderLoop(false)
	, m_drawMode(GL_FILL)
	, m_camera(settings.windowWidth, settings.windowHeight)
{
	// Specify OpenGL context profile hints
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, settings.glVersionMajor);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, settings.glVersionMinor);
	glfwWindowHint(GLFW_OPENGL_PROFILE, settings.glProfile);

	// Request a new window from GLFW
	auto futureWindow = GlfwWindowManager::requestWindow(m_camera.viewportSize().x, m_camera.viewportSize().y, "Simulation", nullptr, nullptr);
	GlfwWindowManager::processEvents();
	m_window = futureWindow.get();
	if (!m_window) throw std::runtime_error("RenderWindow could not be initialized.");
	glfwSetWindowUserPointer(m_window, static_cast<void*>(this));

	// Register all callbacks of this class to the central GlfwWindowManager
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
	auto contextScope = GlfwScopedContextSwitcher(m_window);
	CommonOpenGl::loadOpenGl();
	glfwSwapInterval(1);

	// Initialize window content
	if (!initialize()) std::cerr << "Simulation could not be initialized." << "\n";
}

GlfwRenderWindow::~GlfwRenderWindow()
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

bool GlfwRenderWindow::initialize()
{
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	return true;
}

void GlfwRenderWindow::cleanup()
{
	clearScenes();
}

void GlfwRenderWindow::addScene(Scene* scene)
{
	const auto previousContext = glfwGetCurrentContext();
	if (previousContext != m_window) glfwMakeContextCurrent(m_window);

	m_scenes.push_back(scene);
	scene->setCamera(&m_camera);
	scene->initialize(m_window);

	if (previousContext != m_window) glfwMakeContextCurrent(previousContext);
}

void GlfwRenderWindow::clearScenes()
{
	const auto previousContext = glfwGetCurrentContext();
	if (previousContext != m_window) glfwMakeContextCurrent(m_window);

	for (auto scene : m_scenes) if (scene->isInitialized()) scene->cleanup();
	m_scenes.clear();

	if (previousContext != m_window) glfwMakeContextCurrent(previousContext);
}

void GlfwRenderWindow::executeRenderLoop()
{
	if (m_continueRenderLoop) return;

	auto contextScope = GlfwScopedContextSwitcher(m_window);

	// Run render loop
	m_continueRenderLoop = true;
	while (m_continueRenderLoop && !glfwWindowShouldClose(m_window)) render();
	m_continueRenderLoop = false;
}

void GlfwRenderWindow::requestStopRenderLoop()
{
	m_continueRenderLoop = false;
}

void GlfwRenderWindow::setDebuggingEnabled(bool enabled)
{
	auto contextScope = GlfwScopedContextSwitcher(m_window);

	if (enabled) {
		glEnable(GL_DEBUG_OUTPUT);
		glDebugMessageCallback(&debug_callback, nullptr);
	} else {
		glDisable(GL_DEBUG_OUTPUT);
	}
}

Camera* GlfwRenderWindow::camera()
{
	return &m_camera;
}

void GlfwRenderWindow::render()
{
	glViewport(0, 0, m_camera.viewportSize().x, m_camera.viewportSize().y);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPolygonMode(GL_FRONT_AND_BACK, m_drawMode);

	//! Render the current scenes
	for (auto scene : m_scenes) scene->render();

	glfwSwapBuffers(m_window);
}

void GlfwRenderWindow::mouse_button_callback(GLFWwindow* glfwWindow, int button, int action, int mods)
{
	auto window = static_cast<GlfwRenderWindow*>(glfwGetWindowUserPointer(glfwWindow));

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

void GlfwRenderWindow::cursor_position_callback(GLFWwindow* glfwWindow, double xpos, double ypos)
{
	auto window = static_cast<GlfwRenderWindow*>(glfwGetWindowUserPointer(glfwWindow));
	const int width = window->m_camera.viewportSize().x;
	const int height = window->m_camera.viewportSize().y;

	if (window->m_interaction.pressedButton == GLFW_MOUSE_BUTTON_LEFT) {
		glm::dvec3 oldMouseVec(  window->m_interaction.lastMousePos.x - 0.5 * width,
							   -(window->m_interaction.lastMousePos.y - 0.5 * height), 0);
		glm::dvec3 newMouseVec(  xpos - 0.5 * width,
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

void GlfwRenderWindow::scroll_callback(GLFWwindow* glfwWindow, double xoffset, double yoffset)
{
	auto window = static_cast<GlfwRenderWindow*>(glfwGetWindowUserPointer(glfwWindow));

	bool eventCaptured = false;
	for (auto scene : window->m_scenes) {
		auto sceneGlfwScrollFun = scene->glfwScrollFun();
		if (sceneGlfwScrollFun != nullptr) eventCaptured = sceneGlfwScrollFun(glfwWindow, xoffset, yoffset);
		if (eventCaptured) break;
	}

	if (!eventCaptured) {
		window->m_camera.scale((yoffset > 0) ? 1.1 : 0.9);
	}
}

void GlfwRenderWindow::key_callback(GLFWwindow* glfwWindow, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) glfwSetWindowShouldClose(glfwWindow, GLFW_TRUE);

	auto window = static_cast<GlfwRenderWindow*>(glfwGetWindowUserPointer(glfwWindow));

	bool eventCaptured = false;
	for (auto scene : window->m_scenes) {
		auto sceneGlfwKeyFun = scene->glfwKeyFun();
		if (sceneGlfwKeyFun != nullptr) eventCaptured = sceneGlfwKeyFun(glfwWindow, key, scancode, action, mods);
		if (eventCaptured) break;
	}
}

void GlfwRenderWindow::character_callback(GLFWwindow* glfwWindow, unsigned int codepoint)
{
	auto window = static_cast<GlfwRenderWindow*>(glfwGetWindowUserPointer(glfwWindow));

	bool eventCaptured = false;
	for (auto scene : window->m_scenes) {
		auto sceneGlfwCharFun = scene->glfwCharFun();
		if (sceneGlfwCharFun != nullptr) eventCaptured = sceneGlfwCharFun(glfwWindow, codepoint);
		if (eventCaptured) break;
	}
}

void GlfwRenderWindow::charmods_callback(GLFWwindow* glfwWindow, unsigned int codepoint, int mods) {}

void GlfwRenderWindow::window_size_callback(GLFWwindow* glfwWindow, int width, int height)
{
	auto window = static_cast<GlfwRenderWindow*>(glfwGetWindowUserPointer(glfwWindow));
	window->m_camera.setViewportSize(width, height);
}

void GlfwRenderWindow::debug_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
{
	switch (severity) {
	case GL_DEBUG_SEVERITY_HIGH:
	case GL_DEBUG_SEVERITY_MEDIUM:
		std::cerr << "OpenGL Error: " << message << "\n";
		break;
	case GL_DEBUG_SEVERITY_LOW:
		std::cout << "OpenGL Warning: " << message << "\n";
		break;
	}
}
