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
	initializeImpl();
}

void Scene::render()
{
	assert(m_initialized);
	renderImpl();
}

GLFWwindow* Scene::window()
{
	return m_window;
}
