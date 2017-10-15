#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <noname_tools/range_tools.h>
#include <noname_tools/typetraits_tools.h>

#include "MathHelper.h"

namespace CommonOpenGl {

//! Loads the OpenGL functions using GLAD.
inline void loadOpenGl()
{
	gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress));
}

//! Returns whether the supplied moe is a valid OpenGL primitive rendering mode.
inline bool isValidGlMode(GLenum mode)
{
	static const GLenum glModes[] = {
		GL_POINTS,
		GL_LINE_STRIP,
		GL_LINE_LOOP,
		GL_LINES,
		GL_LINE_STRIP_ADJACENCY,
		GL_LINES_ADJACENCY,
		GL_TRIANGLE_STRIP,
		GL_TRIANGLE_FAN,
		GL_TRIANGLES,
		GL_TRIANGLE_STRIP_ADJACENCY,
		GL_TRIANGLES_ADJACENCY,
		GL_PATCHES
	};

	for (GLenum glMode : noname::tools::make_range(glModes))
		if (glMode == mode) return true;

	return false;
}

//! Calls a glGet*v function corresponding to the template parameter and returns the obtained valued.
template <typename T>
inline T getGlValue(GLenum)
{
	static_assert(noname::tools::dependent_false<T>::value, "There is no glGet* function for this type");
}

//! Calls a glGetBooleanv for the specified parameter and returns the obtained valued.
template <>
inline GLboolean getGlValue<GLboolean>(GLenum pname)
{
	GLboolean data;
	glGetBooleanv(pname, &data);
	return data;
}

//! Calls a glGetDoublev for the specified parameter and returns the obtained valued.
template <>
inline GLdouble getGlValue<GLdouble>(GLenum pname)
{
	GLdouble data;
	glGetDoublev(pname, &data);
	return data;
}

//! Calls a glGetFloatv for the specified parameter and returns the obtained valued.
template <>
inline GLfloat getGlValue<GLfloat>(GLenum pname)
{
	GLfloat data;
	glGetFloatv(pname, &data);
	return data;
}

//! Calls a glGetIntegerv for the specified parameter and returns the obtained valued.
template <>
inline GLint getGlValue<GLint>(GLenum pname)
{
	GLint data;
	glGetIntegerv(pname, &data);
	return data;
}

//! Calls a glGetInteger64v for the specified parameter and returns the obtained valued.
template <>
inline GLint64 getGlValue<GLint64>(GLenum pname)
{
	GLint64 data;
	glGetInteger64v(pname, &data);
	return data;
}


}
