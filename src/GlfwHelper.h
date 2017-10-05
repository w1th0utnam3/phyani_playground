#pragma once

#include <GLFW/glfw3.h>

//! RAII-style GLFW context switcher
/*
 * Switches to the context supplied at construction and switches back to the previous
 * context at destruction.
 */
class GlfwScopedContextSwitcher
{
public:
	//! Switches to the supplied GLFW context.
	GlfwScopedContextSwitcher(GLFWwindow* newContext)
		: m_previousContext(glfwGetCurrentContext())
		, m_newContext(newContext)
	{
		if (m_previousContext != m_newContext) glfwMakeContextCurrent(m_newContext);
	}

	//! Destructor which switches back to the previous context.
	~GlfwScopedContextSwitcher()
	{
		if (m_previousContext != m_newContext) glfwMakeContextCurrent(m_previousContext);
	}

private:
	GLFWwindow* m_previousContext;
	GLFWwindow* m_newContext;
};
