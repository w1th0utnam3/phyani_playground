#include "scene.h"

#include <cassert>

Scene::Scene()
	: m_initialized(false)
	, m_camera(nullptr)
	, m_window(nullptr)
	, m_glfwMouseButtonFun(nullptr)
	, m_glfwScrollFun(nullptr)
	, m_glfwKeyFun(nullptr)
	, m_glfwCharFun(nullptr)
{
}

void Scene::initialize(GLFWwindow* window)
{
	assert(!m_initialized);

	m_initialized = true;
	m_window = window;
	initializeSceneContent();
}

bool Scene::isInitialized() const
{
	return m_initialized;
}

void Scene::cleanup()
{
	assert(m_initialized);

	cleanupSceneContent();
	m_initialized = false;
	m_window = nullptr;
}

void Scene::render()
{
	assert(m_initialized);
	renderSceneContent();
}

GLFWwindow* Scene::window()
{
	return m_window;
}

Camera* Scene::camera()
{
	return m_camera;
}

void Scene::setCamera(Camera* camera)
{
	m_camera = camera;
	cameraUpdated();
}

GLFWmousebuttonfun Scene::glfwMouseButtonFun() const
{
	return m_glfwMouseButtonFun;
}

GLFWscrollfun Scene::glfwScrollFun() const
{
	return m_glfwScrollFun;
}

GLFWkeyfun Scene::glfwKeyFun() const
{
	return m_glfwKeyFun;
}

GLFWcharfun Scene::glfwCharFun() const
{
	return m_glfwCharFun;
}
