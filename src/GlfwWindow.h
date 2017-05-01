#pragma once

#include <atomic>
#include <future>
#include <vector>
#include <memory>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <AntTweakBar.h>

#include "Scene.h"

struct Interaction
{
	glm::dvec2 lastMousePos = glm::dvec2(0, 0);
	int pressedButton = -1;
};

class GlfwWindow
{
public:
	GlfwWindow();
	~GlfwWindow();

	void executeRenderLoop();

	std::future<void> stopRenderLoop();

private:
	std::atomic<bool> m_continueRenderLoop;
	int m_drawMode;
	double m_lastFrametime;
	double m_fps;

	bool initialize();
	void cleanup();

	void timeStep();
	void render();

	static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
	static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
	static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
	static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void window_size_callback(GLFWwindow* window, int width, int height);

	GLFWwindow* m_window;
	TwBar* m_tweakBar;

	Camera m_camera;
	Interaction m_interaction;
	std::vector<std::unique_ptr<Scene>> m_scenes;
};
