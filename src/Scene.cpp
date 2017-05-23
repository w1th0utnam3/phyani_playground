#include "scene.h"

#include <cassert>
#include <iostream>

#include "glad/glad.h"
#include "GLFW/glfw3.h"

Scene::Scene()
	: m_initialized(false)
	, m_window(nullptr)
{
}

void Scene::initialize(GLFWwindow* window)
{
	assert(!m_initialized);

	m_initialized = true;
	m_window = window;
	initializeSceneContent();
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
}
