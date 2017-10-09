#include "Scene.h"

#include <cassert>

Scene::Scene()
	: m_initialized(false)
	, m_camera(nullptr)
	, m_window(nullptr)
	, m_glfwMouseButtonFun(nullptr)
	, m_glfwCursorPosFun(nullptr)
	, m_glfwScrollFun(nullptr)
	, m_glfwKeyFun(nullptr)
	, m_glfwCharFun(nullptr)
	, m_glfwCharModsFun(nullptr)
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

Scene::GLFWmousebuttonfun_bool Scene::glfwMouseButtonFun() const
{
	return m_glfwMouseButtonFun;
}

Scene::GLFWcursorposfun_bool Scene::glfwCursorPosFun() const
{
	return m_glfwCursorPosFun;
}

Scene::GLFWscrollfun_bool Scene::glfwScrollFun() const
{
	return m_glfwScrollFun;
}

Scene::GLFWkeyfun_bool Scene::glfwKeyFun() const
{
	return m_glfwKeyFun;
}

Scene::GLFWcharfun_bool Scene::glfwCharFun() const
{
	return m_glfwCharFun;
}

Scene::GLFWcharmodsfun_bool Scene::glfwCharModsFun() const
{
	return m_glfwCharModsFun;
}
