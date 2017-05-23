#pragma once

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

protected:
	//! Method that subclasses should override to initialize buffers, shaders, etc.
	virtual void initializeSceneContent() {};
	//! Method that subclasses should override to unload shaders, free memory, etc.
	virtual void cleanupSceneContent() {};
	//! Method that subclasses should override to render the specific scene
	virtual void renderSceneContent() {};

	//! Camera of the Scene
	Camera* m_camera;

private:
	//! Flag indicating whether the Scene was initialized
	bool m_initialized;
	//! Associated window of the Scene
	GLFWwindow* m_window;
};
