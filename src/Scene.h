#pragma once

#include <GLFW/glfw3.h>
struct GLFWwindow;

#include "Camera.h"

//! Base class of scenes that can be rendered in an OpenGL context
class Scene
{
public:
	Scene();
	//! Virtual destructor.
	virtual ~Scene() = default;

	//! Initializes the Scene after initialization of the OpenGL context and before first render.
	void initialize(GLFWwindow* window);
	//! Returns whether the scene is initialized.
	bool isInitialized() const;
	//! Frees resources of the scene, called after the last render.
	void cleanup();
	//! Renders the Scene in the render loop.
	void render();
	
	//! Returns the window of the Scene.
	GLFWwindow* window();

	//! Returns the camera of the Scene.
	Camera* camera();
	//! Sets the camera of the Scene.
	void setCamera(Camera* camera);

	using GLFWmousebuttonfun_bool = bool(*)(GLFWwindow*, int, int, int);
	using GLFWcursorposfun_bool = bool(*)(GLFWwindow*, double, double);
	using GLFWscrollfun_bool = bool(*)(GLFWwindow*, double, double);
	using GLFWkeyfun_bool = bool(*)(GLFWwindow*, int, int, int, int);
	using GLFWcharfun_bool = bool(*)(GLFWwindow*, unsigned int);
	using GLFWcharmodsfun_bool = bool(*)(GLFWwindow*, unsigned int, int);

	GLFWmousebuttonfun_bool glfwMouseButtonFun() const;
	GLFWcursorposfun_bool glfwCursorPosFun() const;
	GLFWscrollfun_bool glfwScrollFun() const;
	GLFWkeyfun_bool glfwKeyFun() const;
	GLFWcharfun_bool glfwCharFun() const;
	GLFWcharmodsfun_bool glfwCharModsFun() const;

protected:
	//! Method that subclasses should override to initialize buffers, shaders, etc.
	virtual void initializeSceneContent() {}
	//! Method that subclasses should override to unload shaders, free memory, etc.
	virtual void cleanupSceneContent() {}
	//! Method that subclasses should override to render the specific scene.
	virtual void renderSceneContent() {}
	//! Method that subclasses should override to update scene data after the camera was changed.
	virtual void cameraUpdated() {}

	//! Flag indicating whether the Scene was initialized
	bool m_initialized;
	//! Camera of the Scene
	Camera* m_camera;
	//! Associated window of the Scene
	GLFWwindow* m_window;

	GLFWmousebuttonfun_bool m_glfwMouseButtonFun;
	GLFWcursorposfun_bool m_glfwCursorPosFun;
	GLFWscrollfun_bool m_glfwScrollFun;
	GLFWkeyfun_bool m_glfwKeyFun;
	GLFWcharfun_bool m_glfwCharFun;
	GLFWcharmodsfun_bool m_glfwCharModsFun;
};
