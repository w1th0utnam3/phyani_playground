#pragma once

#include <atomic>
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

// TODO: Move AntTweakBar stuff to own class that can handle threads and switches between windows?

class RenderWindow
{
public:
	//! Constructs a GLFW based render window and initiailizes it.
	RenderWindow();
	//! Constructs a GLFW based render window and initiailizes and requests the specified OpenGL version.
	RenderWindow(int glVersionMajor, int glVersionMinor);
	//! Destroys the GLFW render window and calls cleanup methods.
	~RenderWindow();

	//! Sets the scene that should be rendered in the render window.
	void setScene(std::unique_ptr<Scene>&& scene);

	//! Starts the render loop of this window in the current thread.
	void executeRenderLoop();
	//! Sets a flag to stop the render loop at the next iteration.
	void requestStopRenderLoop();

private:
	//! Initializes the content of the render window.
	bool initialize();
	//! Performs cleanup of any rendering resources on destructions.
	void cleanup();
	//! The render method called in every iteration of the render loop.
	void render();

	//! GLFW callback for mouse clicks.
	static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
	//! GLFW callback for mouse movement.
	static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
	//! GLFW callback for mouse scrolling.
	static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
	//! GLFW callback for key presses.
	static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
	//! GLFW callback for character input.
	static void charmods_callback(GLFWwindow* window, unsigned int codepoint, int mods);
	//! GLFW callback for window resizes.
	static void window_size_callback(GLFWwindow* window, int width, int height);

	//! Flag used to indicate whether the render loop should continue.
	std::atomic<bool> m_continueRenderLoop;

	//! Stores the render mode, i.e. solid or wireframe.
	int m_drawMode;
	//! Timestep increment value for single frame animation increments
	double m_dt;
	//! Current time stretch factor for the animations
	double m_timeStretch;
	//! The time required by the last render loop iteration.
	double m_lastFrametime;
	//! The current number of FPS.
	double m_fps;

	//! Pointer to the underlying GLFWwindow.
	GLFWwindow* m_window;
	//! Pointer to the AntTweakBar context.
	TwBar* m_tweakBar;

	//! Camera settings of the window.
	Camera m_camera;
	//! User interaction data.
	Interaction m_interaction;
	//! Currently loaded scenes that are rendered in the render loop.
	std::unique_ptr<Scene> m_scene;
};
