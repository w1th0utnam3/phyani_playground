#pragma once

struct GLFWwindow;

//! Base class of scenes that can be rendered in an OpenGL context
class Scene
{
public:
	Scene();
	//! Virtual destructor
	virtual ~Scene() = default;

	//! Initializes the Scene after initialization the OpenGL context and before first render
	void initialize(GLFWwindow* window);
	//! Renders the Scene in the render loop
	void render();
	//! Returns the window of the Scene
	GLFWwindow* window();

protected:
	//! Method that subclasses should override to initialize buffers, shaders, etc.
	virtual void initializeImpl() {};
	//! Method that subclasses should override to render the specific scene
	virtual void renderImpl() {};

private:
	//! Flag indicating whether the Scene was initialized
	bool m_initialized;
	//! Associated window of the Scene
	GLFWwindow* m_window;
};
