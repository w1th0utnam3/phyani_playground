#pragma once

#include <atomic>
#include <vector>

#include <GLFW/glfw3.h>

#include "CommonOpenGL.h"
#include "Scene.h"

struct ContextSettings
{
	enum GlProfile {
		AnyProfile = GLFW_OPENGL_ANY_PROFILE,
		CompatibilityProfile = GLFW_OPENGL_COMPAT_PROFILE,
		CoreProfile = GLFW_OPENGL_CORE_PROFILE
	};

	int glVersionMajor = 1;
	int glVersionMinor = 0;
	GlProfile glProfile = GlProfile::AnyProfile;

	int windowWidth = 640;
	int windowHeight = 480;
};

struct Interaction
{
	glm::dvec2 lastMousePos = glm::dvec2(0, 0);
	int pressedButton = -1;
};

class RenderWindow
{
public:
	//! Constructs a GLFW based render window and initiailizes it.
	RenderWindow(const ContextSettings& settings);
	//! Destroys the GLFW render window and calls cleanup methods.
	~RenderWindow();

	//! Adds a scene that should be rendered in the render window.
	void addScene(Scene* scene);
	//! Clears the lits of scenes to render
	void clearScenes();

	//! Starts the render loop of this window in the current thread.
	void executeRenderLoop();
	//! Sets a flag to stop the render loop at the next iteration.
	void requestStopRenderLoop();

	//! Enables or disables OpenGL debugging (prints OpenGL debug messsages to stdandard error)
	void setDebuggingEnabled(bool enabled = true);

	//! Returns a pointer to the camera owned by the window.
	Camera* camera();

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
	static void character_callback(GLFWwindow* window, unsigned int codepoint);
	//! GLFW callback for character input.
	static void charmods_callback(GLFWwindow* window, unsigned int codepoint, int mods);
	//! GLFW callback for window resizes.
	static void window_size_callback(GLFWwindow* window, int width, int height);
	//! OpenGL callback for debug messages.
	static void debug_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);

	//! Flag used to indicate whether the render loop should continue.
	std::atomic<bool> m_continueRenderLoop;

	//! Stores the render mode, i.e. solid or wireframe.
	int m_drawMode;

	//! Pointer to the underlying GLFWwindow.
	GLFWwindow* m_window;

	//! Camera settings of the window.
	Camera m_camera;
	//! User interaction data.
	Interaction m_interaction;
	//! Currently loaded scenes that are rendered in the render loop.
	std::vector<Scene*> m_scenes;
};
