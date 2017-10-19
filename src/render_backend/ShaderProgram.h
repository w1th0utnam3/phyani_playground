#pragma once

#include <string>
#include <vector>

#include "CommonOpenGl.h"

//! Convenience class for loading shaders from files and compiling them
class ShaderProgram
{
public:
	ShaderProgram();
	//! Destructor frees the loaded shaders and program
	~ShaderProgram();

	//! Loads a shader from the specified file and compiles it. Returns whether shader compilation was successful.
	bool loadShader(const std::string& shaderFilename, GLenum shaderType);
	//! Attaches and links all loaded shaders. Returns whether program linking was successful.
	bool createProgram();
	//! Activates this shader program if it isn't already active.
	void useProgram();
	//! Returns the OpenGL index of the shader program.
	GLuint program() const;
	//! Returns the location index of the uniform with the specified name.
	GLuint getUniformLocation(const std::string& name) const;
	//! Returns the location index of the attrib with the specified name.
	GLuint getAttribLocation(const std::string& name) const;

private:
	//! The index of the shader program.
	GLuint m_program;
	//! Indices of all compiled shaders.
	std::vector<GLuint> m_shaders;
};
