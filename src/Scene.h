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

	GLFWmousebuttonfun glfwMouseButtonFun() const;
	GLFWscrollfun glfwScrollFun() const;
	GLFWkeyfun glfwKeyFun() const;
	GLFWcharfun glfwCharFun() const;

protected:
	//! Method that subclasses should override to initialize buffers, shaders, etc.
	virtual void initializeSceneContent() {}
	//! Method that subclasses should override to unload shaders, free memory, etc.
	virtual void cleanupSceneContent() {}
	//! Method that subclasses should override to render the specific scene
	virtual void renderSceneContent() {}
	//! Method that subclasses should override to update scene data after the camera was changed
	virtual void cameraUpdated() {}

	//! Flag indicating whether the Scene was initialized
	bool m_initialized;
	//! Camera of the Scene
	Camera* m_camera;
	//! Associated window of the Scene
	GLFWwindow* m_window;

	GLFWmousebuttonfun m_glfwMouseButtonFun;
	GLFWscrollfun m_glfwScrollFun;
	GLFWkeyfun m_glfwKeyFun;
	GLFWcharfun m_glfwCharFun;
};
