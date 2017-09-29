#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "MathHelper.h"

namespace CommonOpenGL {

inline void loadOpenGL()
{
	gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress));
}

}
