#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <noname_tools/typetraits_tools.h>

#include "MathHelper.h"

namespace CommonOpenGl {

inline void loadOpenGl()
{
	gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress));
}

template <typename T>
inline T getGlValue(GLenum)
{
	static_assert(noname::tools::dependent_false<T>::value, "There is no glGet* function for this type");
}

template <>
inline GLboolean getGlValue<GLboolean>(GLenum pname)
{
	GLboolean data;
	glGetBooleanv(pname, &data);
	return data;
}

template <>
inline GLdouble getGlValue<GLdouble>(GLenum pname)
{
	GLdouble data;
	glGetDoublev(pname, &data);
	return data;
}

template <>
inline GLfloat getGlValue<GLfloat>(GLenum pname)
{
	GLfloat data;
	glGetFloatv(pname, &data);
	return data;
}

template <>
inline GLint getGlValue<GLint>(GLenum pname)
{
	GLint data;
	glGetIntegerv(pname, &data);
	return data;
}

template <>
inline GLint64 getGlValue<GLint64>(GLenum pname)
{
	GLint64 data;
	glGetInteger64v(pname, &data);
	return data;
}


}
