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
	if (m_initialized) {
		std::cerr << "Scene was already initialized." << "\n";
	}

	m_window = window;
	initializeSceneContent();
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
